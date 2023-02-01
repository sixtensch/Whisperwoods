#include "core.h"
#include "Game.h"

Game::Game()
{
	m_renderer = std::make_unique<Renderer>();
}

Game::~Game()
{
}

void Game::Run()
{
}
