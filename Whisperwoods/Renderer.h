#pragma once

#include "RenderHandler.h"
#include "RenderCore.h"

class Renderer sealed
{
public:
	Renderer();
	~Renderer();

	void InitWindow(uint width, uint height);
	void InitRenderer();

	void UpdateWindow();
	void Draw();

	static Renderer& Get();

private:
	static Renderer* s_singleton;

	unique_ptr<RenderHandler> m_renderHandler;
};

