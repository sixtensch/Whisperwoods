#pragma once

#include "RenderHandler.h"
#include "Window.h"
#include "RenderCore.h"

class Renderer sealed
{
public:
	Renderer(HINSTANCE instance);
	~Renderer();

	void Init(uint width, uint height);

	bool UpdateWindow();
	void Draw();

	static Renderer& Get();

private:
	static Renderer* s_singleton;

	HINSTANCE m_instance;

	shared_ptr<Window> m_window;
	unique_ptr<RenderHandler> m_renderHandler;

public:
	unique_ptr<RenderHandler>& GetHandler() { return m_renderHandler; }
};

