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

	Resources& resources = Resources::Get();

	ModelStaticResource* temp = static_cast<ModelStaticResource*>(resources.GetResource(ResourceTypeModelStatic, "WWM/ShadiiTest.wwm"));
	m_renderCore->CreateVertexBuffer(temp->verticies.Data(), temp->GetVertexByteWidth(), temp->vertexBuffer.GetAddressOf());
	m_renderCore->CreateIndexBuffer(temp->indicies.Data(), sizeof(int) * temp->indicies.Size(), temp->indexBuffer.GetAddressOf());

	ModelRiggedResource* temp2 = static_cast<ModelRiggedResource*>(resources.GetResource(ResourceTypeModelRigged, "WWM/Shadii_Animated.wwm"));
	m_renderCore->CreateVertexBuffer(temp2->verticies.Data(), temp2->GetVertexByteWidth(), temp2->vertexBuffer.GetAddressOf());
	m_renderCore->CreateIndexBuffer(temp2->indicies.Data(), sizeof(int) * temp2->indicies.Size(), temp2->indexBuffer.GetAddressOf());
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();
    m_renderCore->UpdateViewInfo(m_mainCamera);



	// Main scene rendering

	for (int i = 0; i < m_lightsPoint.Size(); i++)
	{
		m_lightsPoint[i].Update(); // TODO
	}

	for (int i = 0; i < m_lightsSpot.Size(); i++)
	{
		m_lightsSpot[i].Update(); // TODO
	}

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

	ModelStaticResource* model = static_cast<ModelStaticResource*>(resources.GetResource(ResourceTypeModelStatic, subpath));

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

	ModelRiggedResource* model = static_cast<ModelRiggedResource*>(resources.GetResource(ResourceTypeModelRigged, subpath));

	shared_ptr<MeshRenderableRigged> newRenderable = make_shared<MeshRenderableRigged>(
		m_renderableIDCounter++,
		model,
		cs::Mat4()
		);

	m_worldRenderables.Add((shared_ptr<WorldRenderable>)newRenderable);

	return newRenderable;
}
