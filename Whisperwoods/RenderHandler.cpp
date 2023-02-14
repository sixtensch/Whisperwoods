#include "core.h"
#include "RenderHandler.h"
#include "Vertex.h"
#include "Resources.h"

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
	
	m_lightDirectional->camera.SetValues(90, 1.0f, LIGHT_NEAR, LIGHT_FAR);
	m_lightDirectional->camera.Update();
	m_lightDirectional->camera.CalculateOrthoProjection(LIGHT_DIRECTIONAL_WIDTH, LIGHT_DIRECTIONAL_HEIGHT);


	m_mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(), 0.01f, 100.0f );
	m_mainCamera.CalculatePerspectiveProjection();
	m_mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window);
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();

	// Cull (TODO)

	// Light updates

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


	// ShadowPass

	ExecuteDraw(m_lightDirectional->camera, true);


	// Main scene rendering

	ExecuteDraw(m_mainCamera, false);


	// Text rendering

	for (int i = 0; i < m_texts.Size(); i++)
	{
		m_renderCore->DrawText(m_texts[i].get()->GetFontPos(), m_texts[i].get()->GetText(), m_texts[i].get()->GetFont(), m_texts[i].get()->GetColor(), m_texts[i].get()->GetOrigin());
	}
}



void RenderHandler::Present()
{
	m_renderCore->EndFrame();
}

void RenderHandler::ExecuteDraw(const Camera& povCamera, bool shadows)
{
	m_renderCore->UpdateViewInfo(povCamera);

	if ( !shadows )
	{
		m_renderCore->TargetBackBuffer();
	}
	else
	{
		m_renderCore->TargetShadowMap();
	}

	for ( int i = 0; i < m_worldRenderables.Size(); i++ )
	{
		if ( m_worldRenderables[i]->enabled )
		{
			m_renderCore->UpdateObjectInfo(m_worldRenderables[i].get());
			m_renderCore->DrawObject(m_worldRenderables[i].get(), shadows);
		}
	}
}

const RenderCore* RenderHandler::GetCore() const
{
	return m_renderCore.get();
}

Camera& RenderHandler::GetCamera()
{
	return m_mainCamera;
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