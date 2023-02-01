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

	m_game = std::make_unique<Game>();
}

Whisperwoods::~Whisperwoods()
{
}

void Whisperwoods::Run()
{
	m_game->Run();
}
