#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void Draw();

private:
	void InitVSConstantBuffers();

private:
	unique_ptr<RenderCore> m_renderCore;

	Camera mainCamera;


	ConstantBuffers cbufferData;
};

