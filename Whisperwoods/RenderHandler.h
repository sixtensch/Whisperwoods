#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void Draw();

private:
	unique_ptr<RenderCore> m_renderCore;

	Camera mainCamera;
};

