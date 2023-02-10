#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"

#include "TextRenderable.h"

// TODO: Dudd include. Only used for getting test sound.
#include "SoundResource.h"

void TestPlay(void*, void*)
{
	// Audio test startup
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.3f, 0, 10, "Duck.mp3");
	testSource.Play();
}

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	m_sound = std::make_unique<Sound>();
	m_debug->CaptureSound(m_sound.get());

	m_config = std::make_unique<Config>();

	m_renderer = std::make_unique<Renderer>(instance);
	m_renderer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

	m_input = std::make_unique<Input>();
	m_input->InputInit(Renderer::GetWindow().Data());

	m_game = std::make_unique<Game>(); 

	m_resources = std::make_unique<Resources>();
	m_resources->LoadAssetDirectory(m_renderer->GetRenderCore());
}

Whisperwoods::~Whisperwoods()
{
}

void Whisperwoods::Run()
{
	// Main frame loop

	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck - Copy.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");



	shared_ptr<MeshRenderableRigged> mesh = Renderer::CreateMeshRigged("Shadii_Animated.wwm");
	shared_ptr<MeshRenderableStatic> mesh2 = Renderer::CreateMeshStatic("ShadiiTest.wwm");
	float rotationY = cs::c_pi * 1.0f;
	mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
	mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
	
	mesh->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiBody.wwmt"));
	mesh2->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiBody.wwmt"));



	// Text

	dx::SimpleMath::Vector2 posTest;
	posTest.x = 1270;
	posTest.y = 710;
	const wchar_t* inputText = L"Shadii";
	cs::Color4f color(0.034f, 0.255f, 0.0f, 1.0f);
	//These are just test values
	


	// Lights

	shared_ptr<PointLight> point = make_shared<PointLight>();
	point->color = cs::Color3f(0x4040FF);
	point->intensity = 0.0f;
	point->transform.position = Vec3(2, 0, 0);
	Renderer::RegisterLight(point);

	shared_ptr<SpotLight> spot = make_shared<SpotLight>();
	spot->color = cs::Color3f(0x40FF40);
	spot->intensity = 0.8f;
	spot->transform.position = Vec3(-0.4f, 0.2f, 0);
	spot->transform.rotation = Quaternion::GetIdentity();
	spot->fovInner = 0.15f;
	spot->fovOuter = 0.2f;
	spot->range = 100.0f;
	Renderer::RegisterLight(spot);

	shared_ptr<DirectionalLight> directional = Renderer::GetDirectionalLight();
	directional->transform.position = { 0, 10, 0 };
	directional->transform.SetRotationEuler({ 0.5f, 0.9f, 0.0f });
	directional->diameter = 20.0f;
	directional->intensity = 0.7f;
	directional->color = cs::Color3f(0xFFFFD0);

	shared_ptr<TextRenderable> text = Renderer::CreateTextRenderable(inputText, posTest, FontDefault, color, { 1.0f, 1.0f });

	int frames = 0;
	cs::Timer deltaTimer;

	for (bool running = true; running; frames++)
	{
		m_debug->ClearFrameTrace();
		m_input->Update();
		running = !m_renderer->UpdateWindow();

		float dTime = deltaTimer.Lap();
		m_debug->CalculateFps(dTime);

		static float dTimeAcc = 0.0f;
		dTimeAcc += dTime;

		Move(dTime);

		m_game->Update();
		m_sound->Update();
		rotationY += 2 * dTime;
		mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
		mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, -rotationY, 0); // cs::c_pi * 0.9f



		// Draw step

		m_renderer->Draw();

//#ifdef WW_DEBUG
		m_renderer->BeginGui();
		m_debug->DrawConsole();
		m_renderer->EndGui();
//#endif

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
	static bool lock = false;
	static Vec3 euler = { 0, 0, 0 };
	static MouseState ms = Input::Get().GetMouseState();
	static MouseState ms2 = Input::Get().GetMouseState();

	Camera& camera = Renderer::GetCamera();

	Vec3 movement = Vec3(0, 0, 0);

	float lookSpeed = 0.0002f;

	if (Input::Get().IsKeybindDown(KeybindForward))		movement.z += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindBackward))	movement.z -= 1.0f;
	if (Input::Get().IsKeybindDown(KeybindRight))		movement.x += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindLeft))		movement.x -= 1.0f;
	if (Input::Get().IsKeybindDown(KeybindUp))			movement.y += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindDown))		movement.y -= 1.0f;

	if (Input::Get().IsKeybindDown(KeybindSprint))		movement *= 2.0f;

	if (Input::Get().GetKeyboardState().R && !Input::Get().GetLastKeyboardState().R)
	{
		lock = !lock;
	}

	if (lock)
	{
		POINT center = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };

		Point2 mouseMove = 
		{
			Input::Get().GetMouseState().x - center.x,
			Input::Get().GetMouseState().y - center.y
		};

		if (mouseMove != Point2 { 0, 0 })
		{
			euler += Vec3(-mouseMove.y * lookSpeed, -mouseMove.x * lookSpeed, 0.0f);
			camera.SetRotation(Quaternion::GetEuler(euler));
		}

		ClientToScreen(Renderer::GetWindow().Data(), &center);
		SetCursorPos(center.x, center.y);
	}

	camera.SetPosition(camera.GetPosition() + Quaternion::GetAxisNormalized({ 0, 1, 0 }, -euler.y) * (movement * dTime));
	camera.Update();
}
