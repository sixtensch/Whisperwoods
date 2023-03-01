#include "core.h"
#include "Renderer.h"

#include "GUI.h"

Renderer* Renderer::s_singleton = nullptr;



Renderer::Renderer(HINSTANCE instance)
{
	if (s_singleton != nullptr)
	{
		EXC("Renderer singleton re-initialization.");
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

//#ifdef WW_DEBUG
	m_gui = make_unique<GUI>(m_renderHandler->GetCore(), true, true);
//#endif

	m_renderHandler->CreateTextRenderable(L"This is tutorial", dx::SimpleMath::Vector2(0, 0), FontDefault, cs::Color4f(1.0f, 1.0f, 1.0f, 1.0f), { 0.0f, 0.0f });



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

void Renderer::SetupEnvironmentAssets()
{
	m_renderHandler->SetupEnvironmentAssets();
}

void Renderer::SetPlayerMatrix(const Mat4& matrix)
{
	s_singleton->m_renderHandler->SetPlayerMatrix( matrix );
}

void Renderer::LoadLevel(LevelResource* level, string image)
{
	s_singleton->m_renderHandler->LoadLevel(level, image);
}

void Renderer::LoadEnvironment(const Level* level)
{
	s_singleton->m_renderHandler->LoadEnvironment(level);
}

void Renderer::UnLoadEnvironment()
{
	s_singleton->m_renderHandler->UnLoadEnvironment();
}

void Renderer::ClearShadowRenderables()
{
	s_singleton->m_renderHandler->ClearShadowRenderables();
}

void Renderer::RegisterShadowRenderable()
{
	s_singleton->m_renderHandler->RegisterLastRenderableAsShadow();
}

void Renderer::ExecuteShadowRender()
{
	s_singleton->m_renderHandler->ExecuteStaticShadowDraw();
}

shared_ptr<MeshRenderableStatic> Renderer::CreateMeshStatic(const string& subpath)
{
	return s_singleton->m_renderHandler->CreateMeshStatic(subpath);
}


shared_ptr<MeshRenderableRigged> Renderer::CreateMeshRigged(const string& subpath)
{
	return s_singleton->m_renderHandler->CreateMeshRigged(subpath);
}



/// </summary>
/// <param name="text">What text you want to render</param> 
/// <param name="fontPos">Where on the screen the text will be shown</param> 
/// <param name="font">Which font (and size) of text</param>
/// <param name="color">Color of the text</param> 
/// <param name="origin">Origin on the text "window", use for alligment</param> 
/// <returns></returns>
shared_ptr<TextRenderable> Renderer::CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin)
{
	return s_singleton->m_renderHandler->CreateTextRenderable(text, fontPos, font, color, origin);
}

shared_ptr<DirectionalLight> Renderer::GetDirectionalLight()
{
	return s_singleton->m_renderHandler->GetDirectionalLight();
}

bool Renderer::RegisterLight(shared_ptr<PointLight> pointLight)
{
	return s_singleton->m_renderHandler->RegisterPointLight(pointLight);
}

bool Renderer::RegisterLight(shared_ptr<SpotLight> spotLight)
{
	return s_singleton->m_renderHandler->RegisterSpotLight(spotLight);
}

Camera& Renderer::GetCamera()
{
	return s_singleton->m_renderHandler->GetCamera();
}

Window& Renderer::GetWindow()
{
	return *(s_singleton->m_window.get());
}

RenderCore* Renderer::GetRenderCore()
{
	return m_renderHandler->GetCore();
}

void Renderer::SetTimelineState(bool future)
{
	switch ( future )
	{
		case false: 
			m_renderHandler.get()->SetTimelineStateCurrent(); 
			break;

		case true:
			m_renderHandler.get()->SetTimelineStateFuture(); 
			break;
	}
}

//
//Renderer& Renderer::Get()
//{
//#ifdef WW_Renderer
//	if (s_singleton == nullptr)
//	{
//		EXC("Renderer singleton not found (is nullptr).");
//	}
//#endif
//
//	return *s_singleton;
//}
