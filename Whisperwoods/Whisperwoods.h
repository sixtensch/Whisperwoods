#pragma once

#include "Game.h"
#include "Resources.h"
#include "Sound.h"
#include "Config.h"
#include "Input.h"
#include "Debug.h"
#include "Player.h"
#include "Enemy.h"

class Whisperwoods sealed
{
public:
	Whisperwoods(HINSTANCE instance);
	~Whisperwoods();

	void Run();
	
	static void GodMode(void*, void*);
	static void PeasantMode(void*, void*);

private:
	static Whisperwoods* s_whisperwoods;

	std::unique_ptr<Debug>			m_debug;
	std::unique_ptr<Resources>		m_resources;
	std::unique_ptr<Sound>			m_sound;
	std::unique_ptr<Config>			m_config;
	std::unique_ptr<Input>			m_input;
	std::unique_ptr<Renderer>		m_renderer;

	std::unique_ptr<Game>		m_game;
};

