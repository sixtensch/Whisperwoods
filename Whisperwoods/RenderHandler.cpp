#include "core.h"
#include "RenderHandler.h"
#include "Vertex.h"
#include "Resources.h"

#include "LevelImporter.h"

#define USE_GPU_PROFILER true
#if USE_GPU_PROFILER
	#define PROFILE_JOB(profileName, call) { m_renderCore->ProfileBegin(profileName); call; m_renderCore->ProfileEnd(profileName); }
#else
	#define PROFILE_JOB(profileName, call) { call; }
#endif

RenderHandler::RenderHandler()
{
	m_renderableIDCounter = 0;
	m_timelineState = TimelineStateCurrent;
	m_envQuadTree.Init(2.0f, -1.0f, 50.0f, 50.0f); //TODO: actual start size


	const dx::BoundingBox BananaPlant = {
		dx::XMFLOAT3(0,0,0),
		dx::XMFLOAT3(1,1,1)
	};
	const dx::BoundingBox BananaPlant2TEMP = {
		dx::XMFLOAT3(0,0,0),
		dx::XMFLOAT3(1,1,1)
	};

	// Add to the list

	for (int i = 0; i < 9; i++)
	{
		boundingVolumes[i] = BananaPlant;
	}

	m_currentLevel = nullptr;
	m_lightAmbientIntensity = 0.5f;
}

RenderHandler::~RenderHandler()
{
}

void RenderHandler::InitCore(shared_ptr<Window> window)
{
	m_lightAmbient = cs::Color3f(0xC0C0FF);
	m_lightAmbientIntensity = 0.25f;

	m_lightDirectional = make_unique<DirectionalLight>();
	m_lightDirectional->diameter = 1000.0f;
	m_lightDirectional->intensity = 0.0f;
	m_lightDirectional->color = cs::Color3f(0xFFFFFF);

	m_mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(), 0.86f, 1000.0f );
	m_mainCamera.CalculatePerspectiveProjection();
	m_mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window);
}

void RenderHandler::LoadLevel(LevelResource* level, string image)
{
	LevelImporter::ImportImage(image, m_renderCore.get(), level);
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();
	m_renderCore->UpdatePlayerInfo(m_playerMatrix, m_worldInfo1, m_worldInfo2);

	m_mainCamera.CalculatePerspectiveProjection();
	m_renderCore->UpdateViewInfo(m_mainCamera);
	

	// Main scene rendering

	for (int i = 0; i < m_lightsPoint.Size(); i++)
	{
		m_lightsPoint[i]->Update(0); // TODO
	}
	for (int i = 0; i < m_lightsSpot.Size(); i++)
	{
		m_lightsSpot[i]->Update(0); // TODO
	}
	m_lightDirectional->Update(0); // TODO: DELTA TIME

	m_renderCore->WriteLights(
		m_lightAmbient, 
		m_lightAmbientIntensity, 
		m_mainCamera, 
		m_lightDirectional, 
		m_lightsPoint, 
		m_lightsSpot, 
		m_fogFocus, m_fogRadius);


	// ShadowPass
	m_renderCore->UpdateViewInfo(m_lightDirectional->camera);
	static std::string shadowPassProfileName = "Shadow Pass";
	PROFILE_JOB(shadowPassProfileName, ExecuteDraw(m_timelineState, true));

	// Main scene rendering
	m_renderCore->UpdateViewInfo(m_mainCamera);
	m_renderCore->SetFuture(m_timelineState);
	QuadCull(m_mainCamera);

	static std::string zPrepassProfileName = "Z Prepass Draw";
	static std::string terrainProfileName = "Terrain Draw";
	static std::string mainSceneProfileName = "Main Scene Draw";
	m_renderCore->TargetRenderTexture();
	PROFILE_JOB(zPrepassProfileName, ZPrepass(m_timelineState));
	m_renderCore->TargetRenderTexture();
	PROFILE_JOB( terrainProfileName, RenderTerrain() );
	PROFILE_JOB( mainSceneProfileName, ExecuteDraw(m_timelineState, false) );
	m_renderCore->UnbindRenderTexture();

	// PPFX / FX
	
	static std::string ppfxBloomProfileName = "PPFX Bloom";
	PROFILE_JOB(ppfxBloomProfileName, m_renderCore->DrawPPFX());
	
	// Draw final image to back buffer with tone mapping.
	{
		static std::string backBufferDrawProfileName = "Back Buffer Draw";
		PROFILE_JOB(backBufferDrawProfileName, m_renderCore->DrawToBackBuffer());
	}
	
	// Render text
	{	
		// TODO: Move binding back buffer to OM here instead of it being at the end of DrawToBackBuffer().
		for (int i = 0; i < m_texts.Size(); i++)
		{
			m_renderCore->DrawText(m_texts[i].get()->GetFontPos(), m_texts[i].get()->GetText(), m_texts[i].get()->GetFont(), m_texts[i].get()->GetColor(), m_texts[i].get()->GetOrigin());
		}
	}
	
	// Draw GUI
	static std::string guiDrawProfileName = "GUI Draw calls";
	PROFILE_JOB(guiDrawProfileName, RenderGUI());
}

void RenderHandler::UpdateGPUProfiler()
{
	m_renderCore->UpdateGPUProfiler();
}

void RenderHandler::Present()
{
	m_renderCore->EndFrame();
}

void RenderHandler::ExecuteDraw(TimelineState state, bool shadows)
{
	if ( !shadows )
	{
		m_renderCore->TargetRenderTexture();
	}
	else
	{
		m_renderCore->TargetShadowMap();
	}

	for ( int i = 0; i < m_worldRenderables.Size(); i++ )
	{
		if (shadows)
		{
			bool staticThing = false;
			for (int j = 0; j < m_shadowRenderables.Size(); j++)
			{
				if (m_shadowRenderables[j] == i)
					staticThing = true;
			}
			if (staticThing)
				continue;
		}
		shared_ptr<WorldRenderable> data = {};
		switch ( state )
		{
			case TimelineStateCurrent: 
				data = m_worldRenderables[i].first;
				break;

			case TimelineStateFuture: 
				data = m_worldRenderables[i].second;
				break;
		}
		if ( data && data->enabled )
		{
			m_renderCore->UpdateObjectInfo(data.get());
			m_renderCore->DrawObject(data.get(), shadows, false);
		}
	}

	if (!shadows)
	{
		DrawInstances(state, false, false);
	}
}

/*    ("`-''-/").___..--''"`-._
       `6_ 6  )   `-.  (     ).`-.__.`)
       (_Y_.)'  ._   )  `._ `. ``-..-'
     _..`--'_..-_/  /--'_.' ,'
    (il),-''  (li),'  ((!.-'   <(~Draw all the GUI renderables~)	*/
void RenderHandler::RenderGUI()
{
	for (int i = 0; i < m_guiRenderables.Size(); i++)
	{
		auto data = m_guiRenderables[i];
		if (data && data->enabled)
		{
			m_renderCore->UpdateGUIInfo(data.get()->m_elementRef);
			m_renderCore->UpdateObjectInfo(data.get());
			m_renderCore->DrawObject(data.get(), false, false);
		}
	}
}

void RenderHandler::RenderTerrain()
{
	for (int i = 0; i < m_worldTerrainRenderables.Size(); i++)
	{
		auto data = m_worldTerrainRenderables[i];
		if (data && data->enabled)
		{
			m_renderCore->UpdateObjectInfo( data.get() );
			m_renderCore->DrawObject( data.get(), false, false );
		}
	}
}

void RenderHandler::ZPrepass(TimelineState state)
{
	// Setup
	m_renderCore->TargetPrepass();

	// Draw to the prepass
	DrawInstances(state, true, true);
	for ( int i = 0; i < m_worldRenderables.Size(); i++ )
	{
		shared_ptr<WorldRenderable> data = {};
		switch ( state )
		{
			case TimelineStateCurrent:
				data = m_worldRenderables[i].first;
				break;

			case TimelineStateFuture:
				data = m_worldRenderables[i].second;
				break;
		}
		if ( data && data->enabled )
		{
			m_renderCore->UpdateObjectInfo(data.get());
			m_renderCore->DrawObject(data.get(), true, false);
		}
	}
}

void RenderHandler::ExecuteStaticShadowDraw()
{
	for (uint i = 0; i < LevelAssetCount; i++)
		m_envMeshes[i].hotInstances.MassAdd(m_envMeshes[i].instances.Data(), m_envMeshes[i].instances.Size(), true);
	m_renderCore->UpdateViewInfo(m_lightDirectional->camera);
	m_renderCore->UpdatePlayerInfo(m_playerMatrix, Vec4(0, 0, 0, 0), Vec4(0, 0, 0, 0));


	// Write to static shadow map (Present)
	m_renderCore->TargetStaticShadowMap();
	for (int i = 0; i < m_shadowRenderables.Size(); i++)
	{
		shared_ptr<WorldRenderable> data = m_worldRenderables[m_shadowRenderables[i]].first;
		if (data && data->enabled)
		{
			m_renderCore->UpdateObjectInfo(data.get());
			m_renderCore->DrawObject(data.get(), true, false);
		}
	}
	/*for (int i = 0; i < m_worldTerrainRenderables.Size(); i++)
	{
		auto data = m_worldTerrainRenderables[i];
		if (data && data->enabled)
		{
			m_renderCore->UpdateObjectInfo( data.get() );
			m_renderCore->DrawObject( data.get(), false );
		}
	}*/
	DrawInstances(0, true, true);


	// Write to static shadow map (Future)
	m_renderCore->TargetStaticShadowMapFuture();
	for ( int i = 0; i < m_shadowRenderables.Size(); i++ )
	{
		shared_ptr<WorldRenderable> data = m_worldRenderables[m_shadowRenderables[i]].second;
		if ( data && data->enabled )
		{
			m_renderCore->UpdateObjectInfo(data.get());
			m_renderCore->DrawObject(data.get(), true, false);
		}
	}
	DrawInstances(1, true, true);


	// Set static shadow as readable
	m_renderCore->BindStaticShadowMap(false);
}

RenderCore* RenderHandler::GetCore() const
{
	return m_renderCore.get();
}

Camera& RenderHandler::GetCamera()
{
	return m_mainCamera;
}

void RenderHandler::SetupEnvironmentAssets()
{
	struct DuoModel
	{
		// [0]: Normal, [1]: Future
		const ModelStaticResource* models[2];
		cs::List<string> materials[2];
		LevelAsset asset;
	};

	DuoModel models[LevelAssetCount];



	// Load model primers

	auto load = [&](LevelAsset asset, const char* normal, const cs::List<const char*>& normalMaterials, const char* future, const cs::List<const char*>& futureMaterials)
	{
		models[asset].asset = asset;

		Resources& resources = Resources::Get();
		models[asset].models[0] = resources.GetModelStatic(normal);
		models[asset].models[1] = resources.GetModelStatic(future);

		for (int i = 0; i < normalMaterials.Size(); i++)	models[asset].materials[0].Add(normalMaterials[i]);
		for (int i = 0; i < futureMaterials.Size(); i++)	models[asset].materials[1].Add(futureMaterials[i]);
	};

	load(LevelAssetBush1, 
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" },
		"BananaPlant.wwm", { });

	load(LevelAssetBush2, 
		 "ShadiiTest.wwm", { "ShadiiBody.wwmt", "ShadiiWhite.wwmt", "ShadiiPupil.wwmt" },
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" });

	load(LevelAssetTree1,
		"Medium_Tree_1_Present.wwm", {"Brown_Bark_Tiled.wwmt", "Trees_Foliage.wwmt" },
		"Medium_Tree_1_Future.wwm", {"Tree_Charred_Tiled.wwmt"});

	load(LevelAssetTree2,
		"Medium_Tree_2_Present.wwm", { "Pink_Palm_Tiled.wwmt", "Trees_Foliage.wwmt" },
		"Medium_Tree_2_Future.wwm", { "Tree_Charred_Tiled.wwmt" });

	load(LevelAssetTree3,
		"Medium_Tree_3_Present.wwm", { "Trees_Foliage.wwmt", "Willow_Tree_Tiled.wwmt" },
		"Medium_Tree_3_Future.wwm", { "Tree_Charred_Tiled.wwmt" });

	load(LevelAssetBigTrunk1,
		"Big_Trunk_1.wwm", { "Brown_Bark_Tiled.wwmt" },
		"Big_Trunk_1.wwm", { "Tree_Charred_Tiled.wwmt" });

	load(LevelAssetBigTrunk2,
		"Big_Trunk_2.wwm", { "Brown_Bark_Tiled.wwmt" },
		"Big_Trunk_2.wwm", { "Tree_Charred_Tiled.wwmt" });

	load(LevelAssetStone1,
		"Stone_1_Present.wwm", { "Stone_1_Present.wwmt" },
		"Stone_1_Present.wwm", { "Stone_1_Future.wwmt" });

	load(LevelAssetStone2,
		"Stone_2_Present.wwm", { "Stone_2_Present.wwmt" },
		"Stone_2_Present.wwm", { "Stone_2_Future.wwmt" });



	// Fill global index- and vertex buffers, and setup lists of meshes, submeshes, and materials.

	cs::List<int> allIndices[2];
	cs::List<VertexTextured> allVertices[2];

	for (uint m = 0; m < LevelAssetCount; m++)
	{
		DuoModel& duoModel = models[m];
		EnvMesh& targetMesh = m_envMeshes[duoModel.asset];

		for (uint t = 0; t < 2; t++)
		{
			// Setup references

			cs::List<string>& materials = duoModel.materials[t];

			cs::List<EnvMaterial>& envMaterials = m_envMaterials[t];
			cs::List<EnvSubmesh>& envSubmeshes = m_envSubmeshes[t];

			cs::List<uint>& timeSubmeshes = targetMesh.submeshes[t];

			const ModelStaticResource*& targetModel = targetMesh.models[t];



			// Load model data

			targetModel = duoModel.models[t];

			uint indexStart = allIndices[t].Size();
			uint vertexStart = allVertices[t].Size();

			allIndices[t].MassAdd(targetModel->indicies.Data(), targetModel->indicies.Size(), true);
			allVertices[t].MassAdd(targetModel->verticies.Data(), targetModel->verticies.Size(), true);

			targetMesh.indexStarts[t] = indexStart;
			targetMesh.vertexStarts[t] = vertexStart;

			for (int i = indexStart; i < allIndices[t].Size(); i++)
			{
				allIndices[t][i] += vertexStart;
			}



			// Handle materials

			for (uint materialIndex = 0; materialIndex < (uint)duoModel.materials[t].Size(); materialIndex++)
			{
				string materialName = materials[materialIndex];

				envSubmeshes.Add(
					{ 
						(uint)targetModel->indexCounts[materialIndex], 
						indexStart + (uint)targetModel->startIndicies[materialIndex], 
						duoModel.asset, 
						materialIndex
					});

				uint newSubmeshIndex = envSubmeshes.Size() - 1;

				int foundIndex = -1;
				for (int i = 0; i < m_envMaterials[t].Size(); i++)
				{
					if (envMaterials[i].materialName == materialName)
					{
						foundIndex = i;
						break;
					}
				}

				if (foundIndex == -1)
				{
					envMaterials.Add(
						{
							materialName,
							Resources::Get().GetMaterial(materialName),
							{}
						});

					foundIndex = envMaterials.Size() - 1;
				}

				EnvMaterial& targetMaterial = envMaterials[foundIndex];

				targetMaterial.submeshes.Add(newSubmeshIndex);
				timeSubmeshes.Add(newSubmeshIndex);
			}
		}
	}



	// Finalize buffers

	for (uint t = 0; t < 2; t++)
	{
		m_renderCore->CreateIndexBuffer(allIndices[t].Data(), allIndices[t].Size() * sizeof(int), &m_envIndices[t]);
		m_renderCore->CreateVertexBuffer(allVertices[t].Data(), allVertices[t].Size() * sizeof(VertexTextured), &m_envVertices[t]);
	}
}

void RenderHandler::LoadEnvironment(const Level* level)
{
	m_currentLevel = level;
	
	uint instanceCount = 0;

	m_envQuadTree.Reconstruct((float)level->resource->pixelHeight, (float)level->resource->pixelWidth, level->position);

	for (uint i = 0; i < LevelAssetCount; i++)
	{
		m_envMeshes[i].instances.Clear(false);
		//m_envMeshes[i].instances.MassAdd(level->instances[i].Data(), level->instances[i].Size(), true);
		
		for ( auto& matrix : level->instances[i] )
		{
			m_envMeshes[i].instances.Add(matrix);

			dx::BoundingBox bBox = {};
			// Improvements can be made here, do math for Mat4 transform on boundingVolume instead
			dx::XMMATRIX dxMatrix = dx::XMMatrixTranspose(matrix.XMMatrix());
			boundingVolumes[i].Transform(bBox, dxMatrix);
			shared_ptr<const Mat4*> sptr = make_shared<const Mat4*>(&matrix);

			m_envQuadTree.AddElementIndexed(sptr, bBox, i);
		}

		instanceCount += (uint)level->instances[i].Size();
	}
	

	m_renderCore->CreateInstanceBuffer(nullptr, instanceCount * sizeof(Mat4), &m_envInstanceBuffer);
}

void RenderHandler::UnLoadEnvironment()
{
	m_envInstances.Clear( false );
	for (uint i = 0; i < LevelAssetCount; i++)
	{
		m_envMeshes[i].hotInstances.Clear( false );
	}
	m_worldRenderables.Clear();
	m_worldTerrainRenderables.Clear();
}

void RenderHandler::UpdatePPFXInfo(Vec2 vignette, Vec2 contrast, float brightness, float saturation)
{
	m_renderCore->WritePPFXColorgradeInfo(vignette, contrast, brightness, saturation);
}

shared_ptr<MeshRenderableStatic> RenderHandler::CreateMeshStatic(const string& subpath)
{
	Resources& resources = Resources::Get();

	const ModelStaticResource* model = resources.GetModelStatic(subpath);

	const shared_ptr<MeshRenderableStatic> newRenderable = make_shared<MeshRenderableStatic>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);
	m_worldRenderables.Add({ (shared_ptr<WorldRenderable>)newRenderable, (shared_ptr<WorldRenderable>)newRenderable });

	return newRenderable;
}

shared_ptr<MeshRenderableTerrain> RenderHandler::CreateMeshTerrain( const string& subpath )
{
	Resources& resources = Resources::Get();

	const ModelStaticResource* model = resources.GetModelStatic( subpath );

	const shared_ptr<MeshRenderableTerrain> newRenderable = make_shared<MeshRenderableTerrain>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);
	//m_worldRenderables.Add( { (shared_ptr<WorldRenderable>)newRenderable, (shared_ptr<WorldRenderable>)newRenderable } );
	m_worldTerrainRenderables.Add( shared_ptr<MeshRenderableTerrain>(newRenderable) );
	return newRenderable;
}

std::pair<shared_ptr<MeshRenderableStatic>, shared_ptr<MeshRenderableStatic>> RenderHandler::CreateMeshStaticSwappable(const string& subpathCurrent, const string& subpathFuture)
{
	Resources& resources = Resources::Get();
	const ModelStaticResource* model = resources.GetModelStatic(subpathCurrent);
	shared_ptr<MeshRenderableStatic> renderableCurrent = make_shared<MeshRenderableStatic>(
		m_renderableIDCounter,
		model,
		cs::Mat4()
	);
	model = resources.GetModelStatic(subpathFuture);
	shared_ptr<MeshRenderableStatic> renderableFuture = make_shared<MeshRenderableStatic>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
	);
	std::pair<shared_ptr<MeshRenderableStatic>, shared_ptr<MeshRenderableStatic>> data = {};
	data.first = (shared_ptr<MeshRenderableStatic>)renderableCurrent;
	data.second = (shared_ptr<MeshRenderableStatic>)renderableFuture;
	m_worldRenderables.Add(data);
	return data;
}

shared_ptr<MeshRenderableRigged> RenderHandler::CreateMeshRigged(const string& subpath)
{
	Resources& resources = Resources::Get();

	const ModelRiggedResource* model = resources.GetModelRigged(subpath);

	shared_ptr<MeshRenderableRigged> newRenderable = make_shared<MeshRenderableRigged>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);
	m_worldRenderables.Add({ (shared_ptr<WorldRenderable>)newRenderable , (shared_ptr<WorldRenderable>)newRenderable });
	return newRenderable;
}

shared_ptr<TextRenderable> RenderHandler::CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin)
{
	shared_ptr<TextRenderable> newRenderable = make_shared<TextRenderable>(text, fontPos, font, color, origin);
	m_texts.Add((shared_ptr<TextRenderable>)newRenderable);
	return newRenderable;
}


// Same as static, just different :P, useful for adding things later.
shared_ptr<GUIRenderable> RenderHandler::CreateGUIRenderable(const string& subpath)
{
	Resources& resources = Resources::Get();

	const ModelStaticResource* model = resources.GetModelStatic(subpath);

	const shared_ptr<GUIRenderable> newRenderable = make_shared<GUIRenderable>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);
	m_guiRenderables.Add(newRenderable);

	return newRenderable;
}

void RenderHandler::DestroyMeshStatic(shared_ptr<MeshRenderableStatic> renderable)
{
	for (int i = 0; i < m_worldRenderables.Size(); i++)
	{
		if (m_worldRenderables[i].first == renderable)
		{
			m_worldRenderables.Remove(i);
			return;
		}
	}
}

void RenderHandler::SetTimelineStateCurrent()
{
	m_timelineState = TimelineStateCurrent;
}

void RenderHandler::SetTimelineStateFuture()
{
	m_timelineState = TimelineStateFuture;
}

void RenderHandler::UpdateStaticShadows(bool future)
{
	m_renderCore->BindStaticShadowMap(future);
}

void RenderHandler::SetAmbientLight(cs::Color3f color, float intensity)
{
	m_lightAmbient = color;
	m_lightAmbientIntensity = intensity;
}

shared_ptr<DirectionalLight> RenderHandler::GetDirectionalLight()
{
	return m_lightDirectional;
}

bool RenderHandler::RegisterPointLight(shared_ptr<PointLight> pointLight)
{
	if (m_lightsPoint.Size() >= LIGHT_CAPACITY_POINT)
	{
		return false;
	}

	m_lightsPoint.Add(pointLight);
	return true;
}

bool RenderHandler::RegisterSpotLight(shared_ptr<SpotLight> spotLight)
{
	if (m_lightsSpot.Size() >= LIGHT_CAPACITY_SPOT)
	{
		return false;
	}

	m_lightsSpot.Add(spotLight);
	return true;
}

void RenderHandler::SetPlayerMatrix(const Mat4& matrix)
{
	m_playerMatrix = matrix;
}

void RenderHandler::SetFogParameters(Vec3 focus, float radius)
{
	m_fogFocus = focus;
	m_fogRadius = radius;
}

void RenderHandler::SetWorldParameters( Vec4 worldInfo1, Vec4 worldInfo2 )
{
	m_worldInfo1 = worldInfo1;
	m_worldInfo2 = worldInfo2;
}

void RenderHandler::ClearShadowRenderables()
{
	m_shadowRenderables.Clear();
}

void RenderHandler::RegisterLastRenderableAsShadow()
{
	m_shadowRenderables.Add(m_worldRenderables.Size() - 1);
}

void RenderHandler::QuadCull(const Camera& camPOV)
{
		// Create the view frustum for the culling
	dx::BoundingFrustum viewFrustum = {};
	viewFrustum.CreateFromMatrix(viewFrustum, camPOV.GetProjectionMatrix().XMMatrix());
	Vec3 position = camPOV.GetPosition();
	viewFrustum.Origin = {
		position.x,
		position.y,
		position.z
	};
	Quaternion rotation = camPOV.GetRotation()/*.Conjugate()*/;
	viewFrustum.Orientation = {
		rotation.x,
		rotation.y,
		rotation.z,
		rotation.w
	};

	for ( uint i = 0; i < LevelAssetCount; i++ )
	{
		m_envMeshes[i].hotInstances.Clear(false);
	}

	//for (uint i = 0; i < LevelAssetCount; i++)
	//	m_envMeshes[i].hotInstances.MassAdd(m_envMeshes[i].instances.Data(), m_envMeshes[i].instances.Size(), true);

	m_envQuadTree.CullTreeIndexedQuadrant(viewFrustum, m_envMeshes, 5);
}

void RenderHandler::DrawInstances(uint state, bool shadows, bool discardPipeline)
{
	m_renderCore->BindInstancedPipeline(shadows, discardPipeline);

	m_envInstances.Clear(false);

	for (uint i = 0; i < LevelAssetCount; i++)
	{
		m_envMeshes[i].instanceOffset = m_envInstances.Size();
		m_envMeshes[i].instanceCount = m_envMeshes[i].hotInstances.Size();

		m_envInstances.MassAdd(m_envMeshes[i].hotInstances.Data(), m_envMeshes[i].hotInstances.Size(), true);
	}

	m_renderCore->UpdateInstanceBuffer(m_envInstanceBuffer, m_envInstances.Data(), m_envInstances.Size());
	
	m_renderCore->SetInstanceBuffers(
		m_envVertices[state],
		m_envInstanceBuffer,
		sizeof(VertexTextured),
		sizeof(Mat4),
		0, 0);

	m_renderCore->SetIndexBuffer(m_envIndices[state].Get(), 0);



	// Draw all the stuff!

	for (EnvMaterial& m : m_envMaterials[state])
	{
		m_renderCore->UpdateMaterialInfo(m.material);

		for (uint i : m.submeshes)
		{
			EnvSubmesh& submesh = m_envSubmeshes[state][i];
			EnvMesh& mesh = m_envMeshes[submesh.model];

			m_renderCore->DrawInstanced(submesh.indexCount, mesh.instanceCount, submesh.indexOffset, 0, mesh.instanceOffset);
		}
	}
}
