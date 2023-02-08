#include "core.h"
#include "RenderHandler.h"
#include "Vertex.h"
#include "Resources.h"

RenderHandler::RenderHandler()
{
	m_renderableIDCounter = 0;
}

RenderHandler::~RenderHandler()
{}

void RenderHandler::InitCore(shared_ptr<Window> window)
{
	m_mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(),0.01f, 100.0f );
	m_mainCamera.CalculatePerspectiveProjection();
	m_mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window);

	Resources& resources = Resources::Get();

	ModelStaticResource* temp = static_cast<ModelStaticResource*>(resources.GetResource(ResourceTypeModelStatic, "Characters/ShadiiTest.fbx"));
	temp->CreateVertexBuffer(m_renderCore.get()->GetDeviceP());
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();
    m_renderCore->UpdateViewInfo(m_mainCamera);



	// Main scene rendering

	m_renderCore->TargetBackBuffer();

    for (int i = 0; i < m_worldRenderables.Size(); i++)
    {
        if (m_worldRenderables[i]->enabled)
        {
            m_renderCore->UpdateObjectInfo(m_worldRenderables[i].get());
			m_renderCore->DrawObject(m_worldRenderables[i].get(), false);
        }
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

	ModelStaticResource* model = static_cast<ModelStaticResource*>(resources.GetResource(ResourceTypeModelStatic, "Characters/ShadiiTest.fbx"));

	shared_ptr<MeshRenderableStatic> newRenderable = make_shared<MeshRenderableStatic>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
	);
	
	m_worldRenderables.Add((shared_ptr<WorldRenderable>)newRenderable);

	return newRenderable;
}
