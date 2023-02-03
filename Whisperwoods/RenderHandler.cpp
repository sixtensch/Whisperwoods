#include "core.h"
#include "RenderHandler.h"

RenderHandler::RenderHandler()
{
}

RenderHandler::~RenderHandler()
{
}

void RenderHandler::InitCore(shared_ptr<Window> window)
{
	m_renderCore = make_unique<RenderCore>(window);
	mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(),0.01f, 100.0f );
	mainCamera.CalculatePerspectiveProjection();
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame();
	m_renderCore->EndFrame();
}
