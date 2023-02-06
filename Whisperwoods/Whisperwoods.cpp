#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"

// TODO: Dudd include. Remove later.
//#include "TextureResource.h"

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	m_resources = std::make_unique<Resources>();
	m_sound = std::make_unique<Sound>();

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


	FBXImporter importer;

	ModelRiggedResource riggedModel;

	importer.ImportFBXRigged( "Assets/Shadii_Animated.fbx", &riggedModel );



	// Move this into a model print function
	/*for (size_t i = 0; i < modelPointer->startIndicies.Size(); i++)
	{
		LOG_TRACE("SubMeshMaterial[%d]: %s", i, modelPointer->materialNames[i].c_str());
		for (size_t j = 0; j < modelPointer->indexCounts[i]; j++)
		{
			int indexIndex = modelPointer->startIndicies[i] + j;
			int indicie = modelPointer->indicies[indexIndex];
			Vec3 pos = modelPointer->verticies[indicie].pos;
			Vec3 nor = modelPointer->verticies[indicie].nor;
			LOG_TRACE("Index[%d]: %d Vertex[%d]: pos: (%.2f, %.2f, %.2f) nor: (%.2f, %.2f, %.2f)",
				j,
				indicie,
				indicie,
				pos.x, pos.y, pos.z,
				nor.x, nor.y, nor.z);
		}
	}*/

	int frames = 0;
	for (bool running = true; running; frames++)
	{
		m_debug->ClearFrameTrace();

		running = !m_renderer->UpdateWindow();

		m_game->Update();
		
		m_sound->Update();

		//m_game->Draw();

		m_renderer->Draw();
	}

	// Audio test 2 shutdown
	testSource.pitch = 0.75f;
	testSource.Play();
	while (testSource.IsPlaying())
	{
		m_sound->Update();
	}
}
