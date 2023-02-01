#pragma once

#include "RenderCore.h"

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void Draw();

private:
	unique_ptr<RenderCore> m_renderCore;
};

