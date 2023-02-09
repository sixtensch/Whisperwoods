#include "core.h"
#include "Renderer.h"

#include "GUI.h"

Renderer* Renderer::s_singleton = nullptr;



Renderer::Renderer(HINSTANCE instance)
{
	if (s_singleton != nullptr)
	{
		throw "Renderer singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;

	m_instance = instance;
}

Renderer::~Renderer()
{
	s_singleton = nullptr;
}

void Renderer::Init(uint width, uint height)
{
	m_window = make_shared<Window>(WINDOW_NAME, m_instance, width, height);

	m_renderHandler = make_unique<RenderHandler>();
	m_renderHandler->InitCore(m_window);

	m_gui = make_unique<GUI>(m_renderHandler->GetCore(), true, false);

	m_window->Show(true);
}

bool Renderer::UpdateWindow()
{
	return m_window->PollEvents();
}

void Renderer::Draw()
{
	m_renderHandler->Draw();
}

void Renderer::BeginGui()
{
	m_gui->BeginDraw();
}

void Renderer::EndGui()
{
	m_gui->EndDraw();
}

void Renderer::Present()
{
	m_renderHandler->Present();
}

shared_ptr<MeshRenderableStatic> Renderer::CreateMeshStatic(const string& subpath)
{
	return s_singleton->m_renderHandler->CreateMeshStatic(subpath);
}

shared_ptr<MeshRenderableRigged> Renderer::CreateMeshRigged(const string& subpath)
{
	return s_singleton->m_renderHandler->CreateMeshRigged(subpath);
}

Camera& Renderer::GetCamera()
{
	return s_singleton->m_renderHandler->GetCamera();
}

Window& Renderer::GetWindow()
{
	return *(s_singleton->m_window.get());
}

const RenderCore* Renderer::GetRenderCore()
{
	return m_renderHandler->GetCore();
}

//
//Renderer& Renderer::Get()
//{
//#ifdef WW_Renderer
//	if (s_singleton == nullptr)
//	{
//		throw "Renderer singleton not found.";	// TODO: Proper exceptions
//	}
//#endif
//
//	return *s_singleton;
//}
