#include "core.h"
#include "RenderHandler.h"
#include "Vertex.h"
#include "Resources.h"

// TODO: Testing include for PPFX include
#include "Input.h"

#include "LevelImporter.h"

RenderHandler::RenderHandler()
{
	m_renderableIDCounter = 0;
}

RenderHandler::~RenderHandler()
{
}

void RenderHandler::InitCore(shared_ptr<Window> window)
{
	m_lightAmbient = cs::Color3f(0xD0D0FF);
	m_lightAmbientIntensity = 0.15f;

	m_lightDirectional = make_unique<DirectionalLight>();
	m_lightDirectional->diameter = 100.0f;
	m_lightDirectional->intensity = 0.0f;
	m_lightDirectional->color = cs::Color3f(0xFFFFFF);

	m_mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(), 0.01f, 100.0f );
	m_mainCamera.CalculatePerspectiveProjection();
	m_mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window);
}

void RenderHandler::LoadLevel(LevelResource* level, string image)
{
	LevelImporter::ImportImage("Examplemap.png", m_renderCore.get(), level);

	Mat4 mats[] =
	{
		Mat::translation3(0, 10, 0),
		Mat::translation3(2, 10, 0),
		Mat::translation3(4, 10, 0)
	};

	//m_renderCore->CreateInstanceBuffer(mats, sizeof(Mat4) * 3, &m_instances);
	//m_instance = (const ModelStaticResource*)Resources::Get().GetResource(ResourceTypeModelStatic, "BananaPlant.wwm");
	//m_instanceMaterial = (const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneBanana.wwmt");
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();

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

	m_renderCore->WriteLights(m_lightAmbient, m_lightAmbientIntensity, m_mainCamera, m_lightDirectional, m_lightsPoint, m_lightsSpot);
	m_renderCore->TargetBackBuffer();

	DrawInstances();

	for (int i = 0; i < m_worldRenderables.Size(); i++)
	{
		if (m_worldRenderables[i]->enabled)
		{
			m_renderCore->UpdateObjectInfo(m_worldRenderables[i].get());
			m_renderCore->DrawObject(m_worldRenderables[i].get(), false);
		}
	}

	static bool ppfxOn = false;
	if (Input::Get().IsDXKeyPressed(DXKey::E))
		ppfxOn = !ppfxOn;

	if (ppfxOn)
	{
		m_renderCore->DrawPPFX();
	}
	

	for (int i = 0; i < m_texts.Size(); i++)
	{
		m_renderCore->DrawText(m_texts[i].get()->GetFontPos(), m_texts[i].get()->GetText(), m_texts[i].get()->GetFont(), m_texts[i].get()->GetColor(), m_texts[i].get()->GetOrigin());
	}
}



void RenderHandler::Present()
{
	m_renderCore->EndFrame();
}

const RenderCore* RenderHandler::GetCore() const
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

	auto load = [&](LevelAsset asset, const char* normal, const cs::List<const char*>& normalMaterials, const char* future, const cs::List<const char*>& futureMaterials)
	{
		models[asset].asset = asset;

		models[asset].models[0] = (const ModelStaticResource*)Resources::Get().GetResource(ResourceTypeModelStatic, normal);
		models[asset].models[1] = (const ModelStaticResource*)Resources::Get().GetResource(ResourceTypeModelStatic, future);

		for (int i = 0; i < normalMaterials.Size(); i++)	models[asset].materials[0].Add(normalMaterials[i]);
		for (int i = 0; i < futureMaterials.Size(); i++)	models[asset].materials[1].Add(futureMaterials[i]);
	};

	load(LevelAssetBush1, 
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" },
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" });

	load(LevelAssetBush2,
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" },
		"BananaPlant.wwm", { "TestSceneBanana.wwmt" });

	for (uint m = 0; m < LevelAssetCount; m++)
	{
		DuoModel& duoModel = models[m];
		EnvMesh& targetMesh = m_envMeshes[duoModel.asset];

		for (uint t = 0; t < 2; t++)
		{
			targetMesh.models[t] = duoModel.models[t];
			cs::List<string>& materials = duoModel.materials[t];

			cs::List<EnvMaterial>& envMaterials = m_envMaterials[t];
			cs::List<EnvSubmesh>& envSubmeshes = m_envSubmeshes[t];

			cs::List<uint>& timeSubmeshes = targetMesh.submeshes[t];

			for (uint materialIndex = 0; materialIndex < (uint)duoModel.materials[t].Size(); materialIndex++)
			{
				string materialName = materials[materialIndex];

				envSubmeshes.Add({ 0, duoModel.asset, materialIndex });

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
							nullptr, nullptr,
							materialName,
							(const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, materialName),
							{}
						}
					);

					foundIndex = envMaterials.Size() - 1;
				}

				EnvMaterial& targetMaterial = envMaterials[foundIndex];

				targetMaterial.submeshes.Add(newSubmeshIndex);
				timeSubmeshes.Add(newSubmeshIndex);
			}
		}
	}

	for (uint t = 0; t < 2; t++)
	{
		for (EnvMaterial& material : m_envMaterials[t])
		{
			cs::List<int> allIndices;
			cs::List<VertexTextured> allVertices;

			for (uint submeshIndex : material.submeshes)
			{

			}
		}
	}
}

void RenderHandler::LoadEnvironment(const Level* level)
{
	m_currentLevel = level;

	for (uint i = 0; i < LevelAssetCount; i++)
	{
		m_envMeshes[i].instances.Clear(false);
		m_envMeshes[i].instances.MassAdd(level->instances[i].Data(), level->instances[i].Size(), true);
	}
}

shared_ptr<MeshRenderableStatic> RenderHandler::CreateMeshStatic(const string& subpath)
{
	Resources& resources = Resources::Get();

	const ModelStaticResource* model = static_cast<const ModelStaticResource*>(resources.GetResource(ResourceTypeModelStatic, subpath));

	shared_ptr<MeshRenderableStatic> newRenderable = make_shared<MeshRenderableStatic>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
	);
	
	m_worldRenderables.Add((shared_ptr<WorldRenderable>)newRenderable);

	return newRenderable;
}

shared_ptr<MeshRenderableRigged> RenderHandler::CreateMeshRigged(const string& subpath)
{
	Resources& resources = Resources::Get();

	const ModelRiggedResource* model = static_cast<const ModelRiggedResource*>(resources.GetResource(ResourceTypeModelRigged, subpath));

	shared_ptr<MeshRenderableRigged> newRenderable = make_shared<MeshRenderableRigged>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);

	m_worldRenderables.Add((shared_ptr<WorldRenderable>)newRenderable);

	return newRenderable;
}

shared_ptr<TextRenderable> RenderHandler::CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin)
{
	shared_ptr<TextRenderable> newRenderable = make_shared<TextRenderable>(text, fontPos, font, color, origin);
	m_texts.Add((shared_ptr<TextRenderable>)newRenderable);
	return newRenderable;
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

void RenderHandler::DrawInstances()
{
	m_renderCore->BindInstancedPipeline(false);

	for (uint i = 0; i < m_envMaterials->Size(); i++)
	{

	}

	//m_renderCore->SetInstanceBuffers(
	//	m_instance->vertexBuffer, 
	//	m_instances, 
	//	sizeof(VertexTextured), 
	//	sizeof(Mat4), 
	//	0, 0);

	//m_renderCore->SetIndexBuffer(m_instance->indexBuffer, 0);
	//m_renderCore->UpdateMaterialInfo(m_instanceMaterial);

	//for (int materialIndex = 0; materialIndex < m_instance->startIndicies.Size(); materialIndex++)
	//{
	//	int currentSize = m_instance->indexCounts[materialIndex];
	//	int currentStart = (uint)m_instance->startIndicies[materialIndex];

	//	m_renderCore->DrawInstanced(currentSize, 3, currentStart, 0, 0);
	//}
}
