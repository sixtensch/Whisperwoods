#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"
#include "Animator.h"
#include <imgui.h>
#include "TextRenderable.h"

// TODO: Dudd include. Only used for getting test sound.
#include "SoundResource.h"

void TestPlay(void*, void*)
{
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 0.5f, 0, 10, soundPtr);
	testSource.Play();
}

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	//FBXImporter importer;
	//ModelRiggedResource shadiiAnimated;
	//ModelRiggedResource shadiiAnimated2;
	//ModelRiggedResource shadiiAnimations;
	//importer.ImportFBXRigged( "Assets/Shadii_Animated.fbx", &shadiiAnimated );
	//importer.ImportFBXRigged( "Assets/Shadii_Animated2.fbx", &shadiiAnimated2 );
	//importer.ImportFBXRigged( "Assets/Shadii_Animations.fbx", &shadiiAnimations );
	//std::string path1 = importer.SaveWMM(&shadiiAnimated, "Assets/Models/Rigged/");
	//std::string path2 = importer.SaveWMM( &shadiiAnimated, "Assets/Models/Rigged/" );
	//std::string path3 = importer.SaveWMM( &shadiiAnimated, "Assets/Models/Rigged/" );

	//importer.ImportFBXRigged("Assets/Shadii_Animations.fbx", &riggedModel);
	//path = importer.SaveWMM(&riggedModel, "Assets/Models/Rigged/");

	/*ModelStaticResource staticTestModelWrite;
	importer.ImportFBXStatic( "Assets/Debug_Sphere.fbx", &staticTestModelWrite);
	std::string path4 = importer.SaveWMM(&staticTestModelWrite, "Assets/Models/Static/");*/

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
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");

	shared_ptr<MeshRenderableRigged> mesh = Renderer::CreateMeshRigged("Shadii_Animated.wwm");
	shared_ptr<MeshRenderableStatic> mesh2 = Renderer::CreateMeshStatic("ShadiiTest.wwm");
	//shared_ptr<MeshRenderableStatic> meshSphere = Renderer::CreateMeshStatic("Assets/Models/Static/Debug_Sphere.wwm");


	Resources resources = Resources::Get();

	Animator testAnimator((ModelRiggedResource*)resources.GetResource(ResourceTypeModelRigged, "Shadii_Animated.wwm"));
	ModelRiggedResource* printReference = (ModelRiggedResource*)resources.GetResource(ResourceTypeModelRigged, "Shadii_Animated2.wwm");

	for (int i = 0; i < printReference->armature.bones.Size(); i++)
	{
		std::string boneName = printReference->armature.bones[i].name;
		DirectX::XMFLOAT4X4 m = printReference->armature.bones[i].inverseBindMatrix;
		LOG_TRACE("Inverse Bind Matrix For: %s\n %.2f, %.2f, %.2f, %.2f\n %.2f, %.2f, %.2f, %.2f\n %.2f, %.2f, %.2f, %.2f\n %.2f, %.2f, %.2f, %.2f\n",
			boneName.c_str(),
			m._11, m._12, m._13, m._14,
			m._21, m._22, m._23, m._24,
			m._31, m._32, m._33, m._34,
			m._41, m._42, m._43, m._44)
	}

	//FBXImporter importer;
	FBXImporter importer;
	shared_ptr<AnimationResource> resource (new AnimationResource);
	importer.ImportFBXAnimations("Assets/Models/Shadii_Animations.fbx", resource.get());

	Animation* animation = &resource->animations[2];
	Animation* animation2 = &resource->animations[4];
	Animation* animation3 = &resource->animations[5];
	float speed = 1.5f;
	testAnimator.AddAnimation(animation, 0, speed, 1);
	testAnimator.AddAnimation(animation2, 0, speed, 0.2f);
	testAnimator.AddAnimation( animation3, 0, speed, 0.0f );

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

		Input::Get().Update();
		Move(dTime);

		testAnimator.Update(dTime);

		m_game->Update();
		m_sound->Update();
		rotationY += 0.2f * dTime;
		mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
		mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, -rotationY, 0); // cs::c_pi * 0.9f



		// Draw step

		m_renderer->Draw();

//#ifdef WW_DEBUG
		m_renderer->BeginGui();

		if (ImGui::Begin( "Animation" ))
		{
			ImGui::Text( "The base animation is %s", testAnimator.loadedAnimations[0].sourceAnimation->name.c_str());
			ImGui::SliderFloat( testAnimator.loadedAnimations[1].sourceAnimation->name.c_str(), &testAnimator.loadedAnimations[1].influence, 0.0f, 1.0f, "influence = %.3f");
			ImGui::SliderFloat( testAnimator.loadedAnimations[2].sourceAnimation->name.c_str(), &testAnimator.loadedAnimations[2].influence, 0.0f, 1.0f, "influence = %.3f" );
			ImGui::SliderFloat( "Speed", &speed, 0.0f, 3.0f, "speed = %.3f" );
			testAnimator.loadedAnimations[0].speed = speed;
			testAnimator.loadedAnimations[1].speed = speed;
			testAnimator.loadedAnimations[2].speed = speed;
		}
		ImGui::End();


		m_debug->DrawConsole();
		m_renderer->EndGui();
//#endif

		m_renderer->Present();
	}

	// Audio test 2 shutdown
	testSource.pitch = 0.75f;
	testSource.Play();
	int indexer = 0;
	while (testSource.IsPlaying())
	{
		m_sound->Update();
		indexer++;
	}
}

void Whisperwoods::Move(float dTime)
{
	static bool cameraLock = false;

	Camera& camera = Renderer::GetCamera();

	// TODO: Little ugly. Maybe pretty it up.
	Vec3 movement = Vec3(0, 0, 0);
	Vec3 forwardDirection = camera.GetDirection();
	forwardDirection.y = 0;
	forwardDirection.Normalize();
	Vec3 rightDirection = camera.GetRight();
	Vec3 upDirection = Vec3(0.0f, 1.0f, 0.0f);

	if (Input::Get().IsKeybindDown(KeybindForward))		movement += forwardDirection;
	if (Input::Get().IsKeybindDown(KeybindBackward))	movement -= forwardDirection;
	if (Input::Get().IsKeybindDown(KeybindRight))		movement += rightDirection;
	if (Input::Get().IsKeybindDown(KeybindLeft))		movement -= rightDirection;
	if (Input::Get().IsKeybindDown(KeybindUp))			movement += upDirection;
	if (Input::Get().IsKeybindDown(KeybindDown))		movement -= upDirection;

	if (Input::Get().IsDXKeyPressed(DXKey::R))
	{
		cameraLock = !cameraLock;
	}

	if (Input::Get().IsKeybindDown(KeybindSprint))
	{
		movement *= 5.0f;
	}

	MouseState mouseState = Input::Get().GetMouseState();

	static Vec3 rotationVec = {};
	if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
	{
		cs::Vec3 delta = Vec3(mouseState.y, mouseState.x, 0.0f);

		rotationVec -= delta * dTime * 4.0f;
		camera.SetRotation(Quaternion::GetEuler(rotationVec));
	}

	Input::Get().SetMode(cameraLock ? dx::Mouse::MODE_RELATIVE : dx::Mouse::MODE_ABSOLUTE);

	camera.SetPosition(camera.GetPosition() + Quaternion::GetAxisNormalized({ 0, 1, 0 }, -euler.y) * (movement * dTime));
	camera.Update();
}
