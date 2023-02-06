#pragma once

#include "RenderHandler.h"
#include "Window.h"
#include "RenderCore.h"

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

	static Renderer& Get();

private:
	static Renderer* s_singleton;

	HINSTANCE m_instance;

	shared_ptr<Window> m_window;
	unique_ptr<RenderHandler> m_renderHandler;
	unique_ptr<GUI> m_gui;
};

