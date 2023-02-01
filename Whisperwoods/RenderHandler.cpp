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
}

void RenderHandler::Draw()
{
}
