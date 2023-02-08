#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"

// TODO: Dudd include. Only used for getting test sound.
#include "SoundResource.h"

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

	m_renderer = std::make_unique<Renderer>(instance);
	m_renderer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_input = std::make_unique<Input>();
	m_input->InputInit(Renderer::GetWindow().Data());

	m_game = std::make_unique<Game>(); 


	// TODO: Change to some init place.
	m_resources->LoadAssetDirectory(nullptr);
}

Whisperwoods::~Whisperwoods()
{
}

void Whisperwoods::Run()
{
	// Main frame loop

	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Assets/Sounds/Duck - Copy.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");

	//FBXImporter importer;

	//ModelRiggedResource riggedModel;
	//AnimationResource animationResource;

	//importer.ImportFBXRigged( "Assets/Shadii_Animated.fbx", &riggedModel );
	//importer.ImportFBXAnimations( "Assets/Shadii_Animated.fbx", &animationResource );

	//// Read write test.
	//ModelStaticResource staticTestModelWrite;
	//importer.ImportFBXStatic( "Assets/Models/Characters/ShadiiTest.fbx", &staticTestModelWrite);

	//std::string path = importer.SaveWMM(&staticTestModelWrite);
	//ModelStaticResource staticTestModelRead;
	//importer.LoadWWMStatic(path, &staticTestModelRead);

	//std::string path2 = importer.SaveWMM(&riggedModel);
	//ModelRiggedResource riggedTestModelRead;
	//importer.LoadWWMRigged(path2, &riggedTestModelRead);

	
	shared_ptr<MeshRenderableStatic> mesh = Renderer::CreateMeshStatic("WWM/ShadiiTest.wwm");
	float rotationY = cs::c_pi * 1.0f;
	mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f

	int frames = 0;
	cs::Timer deltaTimer;


	for (bool running = true; running; frames++)
	{
		m_debug->ClearFrameTrace();
		running = !m_renderer->UpdateWindow();

		float dTime = deltaTimer.Lap();
		m_debug->CalculateFps(dTime);

		Move(dTime);

		m_game->Update();
		m_sound->Update();
		rotationY += 2 * dTime;
		mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f

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

void Whisperwoods::Move(float dTime)
{
	Camera& camera = Renderer::GetCamera();

	Vec3 movement = Vec3(0, 0, 0);

	if (Input::Get().IsKeybindDown(KeybindForward))		movement.z += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindBackward))	movement.z -= 1.0f;
	if (Input::Get().IsKeybindDown(KeybindRight))		movement.x += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindLeft))		movement.x -= 1.0f;

	camera.SetPosition(camera.GetPosition() + movement * dTime);
	camera.Update();
}
