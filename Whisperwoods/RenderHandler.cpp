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
	m_lightAmbient = cs::Color3f(0xFFFFFF);
	m_lightAmbientIntensity = 0.5f;

	m_lightDirectional.transform.position = { 0, 10, 0 };
	m_lightDirectional.transform.SetRotationEuler({ 0.5f, 0.9f, 0.0f });
	m_lightDirectional.diameter = 20.0f;
	m_lightDirectional.intensity = 1.0f;
	m_lightDirectional.color = cs::Color3f(0xFFFFB0);

	m_mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(), 0.01f, 100.0f );
	m_mainCamera.CalculatePerspectiveProjection();
	m_mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window);
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();

    m_renderCore->UpdateViewInfo(m_mainCamera);
	
	
	// Main scene rendering

	m_lightDirectional.Update();
	m_renderCore->WriteLights(m_lightAmbient, m_lightAmbientIntensity, m_mainCamera, m_lightDirectional, m_lightsPoint, m_lightsSpot);
	m_renderCore->TargetBackBuffer();

    for (int i = 0; i < m_worldRenderables.Size(); i++)
    {
        if (m_worldRenderables[i]->enabled)
        {
            m_renderCore->UpdateObjectInfo(m_worldRenderables[i].get());
			m_renderCore->DrawObject(m_worldRenderables[i].get(), false);
        }
    }

	for (int i = 0; i < m_texts.Size(); i++)
	{
		m_renderCore->DrawText(m_texts[i].get()->GetFontPos(), m_texts[i].get()->GetText(), m_texts[i].get()->GetFont(), m_texts[i].get()->GetColor());
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

shared_ptr<TextRenderable> RenderHandler::CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color)
{
	shared_ptr<TextRenderable> newRenderable = make_shared<TextRenderable>(text, fontPos, font, color);
	m_texts.Add((shared_ptr<TextRenderable>)newRenderable);
	return newRenderable;
}
