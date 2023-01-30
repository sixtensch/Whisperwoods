#pragma once

#include "Renderer.h"

class Game sealed
{
public:
	Game();
	~Game();

	void Run();

private:
	std::unique_ptr<Renderer>	m_renderer;
};

