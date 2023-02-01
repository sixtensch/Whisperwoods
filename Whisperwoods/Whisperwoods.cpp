#include "core.h"
#include "Whisperwoods.h"

#include "Game.h"

Whisperwoods::Whisperwoods()
{
	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	m_resources = std::make_unique<Resources>();
	m_sound = std::make_unique<Sound>();
	m_config = std::make_unique<Config>();
	m_input = std::make_unique<Input>();

	m_renderer = std::make_unique<Renderer>();
	m_renderer->InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_renderer->InitRenderer();

	m_game = std::make_unique<Game>(); 
}

Whisperwoods::~Whisperwoods()
{
}

void Whisperwoods::Run()
{
	// Main frame loop

	int frames = 0;
	for (bool running = true; running; frames++)
	{
		m_renderer->UpdateWindow();

		m_game->Update();
		//m_game->Draw();

		m_renderer->Draw();
	}
}
