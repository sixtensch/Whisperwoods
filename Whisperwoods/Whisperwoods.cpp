#include "core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"

void TestPlay(void*, void*)
{
	// Audio test startup
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.3f, 0, 10, "Assets/Duck.mp3");
	testSource.Play();
}

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	m_resources = std::make_unique<Resources>();

	m_sound = std::make_unique<Sound>();
	m_debug->CaptureSound(m_sound.get());

	m_config = std::make_unique<Config>();

	m_input = std::make_unique<Input>();
	//m_input->InputInit(HWND windowHandle);
	// TODO: Add so that window handle can be received from renderer/window.

	m_renderer = std::make_unique<Renderer>(instance);
	m_renderer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_game = std::make_unique<Game>(); 
}

Whisperwoods::~Whisperwoods()
{
}

void Whisperwoods::Run()
{
	// Main frame loop

	// Audio test startup
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, "Assets/Duck.mp3");
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");






	int frames = 0;
	for (bool running = true; running; frames++)
	{
		m_debug->ClearFrameTrace();

		running = !m_renderer->UpdateWindow();

		m_game->Update();
		
		m_sound->Update();

		//m_game->Draw();

		m_renderer->Draw();
		m_renderer->BeginGui();
		m_debug->DrawConsole();
		m_renderer->EndGui();
		m_renderer->Present();
	}

	// Audio test 2 shutdown
	testSource.pitch = 0.75f;
	testSource.Play();
	while (testSource.IsPlaying())
	{
		m_sound->Update();
	}
}
