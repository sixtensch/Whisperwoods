#pragma once

#include "RenderHandler.h"
#include "Window.h"
#include "RenderCore.h"
#include "MeshRenderable.h"
#include "TextRenderable.h"
class GUI;

class Renderer sealed
{
public:
	Renderer(HINSTANCE instance);
	~Renderer();

	void Init(uint width, uint height);

	bool UpdateWindow();
	void Draw();
	void BeginGui();
	void EndGui();
	void Present();

	static shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	static shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);

	static shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color);

	static Camera& GetCamera();
	static Window& GetWindow();
	//static Renderer& Get();

private:
	static Renderer* s_singleton;

	HINSTANCE m_instance;

	shared_ptr<Window> m_window;
	unique_ptr<RenderHandler> m_renderHandler;
	unique_ptr<GUI> m_gui;

};

