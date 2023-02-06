#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "ModelResource.h"

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void Draw();

	ModelStaticResource* staticModel;
	//int indexDataSize;
	
private:
	void InitVSConstantBuffers();

private:
	unique_ptr<RenderCore> m_renderCore;

	Camera mainCamera;


	ConstantBuffers cbufferData;

public: 
	unique_ptr<RenderCore>& GetRenderCore() { return m_renderCore; };
};

