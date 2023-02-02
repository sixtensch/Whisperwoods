#include "core.h"
#include "Renderer.h"

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

Renderer& Renderer::Get()
{
#ifdef WW_Renderer
	if (s_singleton == nullptr)
	{
		throw "Renderer singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}
