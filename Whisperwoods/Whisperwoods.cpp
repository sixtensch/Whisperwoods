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
	// Audio test startup
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.3f, 0, 10, "Assets/Duck.mp3");
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
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Assets/Sounds/Duck - Copy.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");

	shared_ptr<MeshRenderableRigged> mesh = Renderer::CreateMeshRigged("Assets/Models/Rigged/Shadii_Animated.wwm");
	shared_ptr<MeshRenderableStatic> mesh2 = Renderer::CreateMeshStatic("Assets/Models/Static/ShadiiTest.wwm");
	//shared_ptr<MeshRenderableStatic> meshSphere = Renderer::CreateMeshStatic("Assets/Models/Static/Debug_Sphere.wwm");


	Resources resources = Resources::Get();

	Animator testAnimator((ModelRiggedResource*)resources.GetResource(ResourceTypeModelRigged, "Assets/Models/Rigged/Shadii_Animated.wwm"));
	ModelRiggedResource* printReference = (ModelRiggedResource*)resources.GetResource(ResourceTypeModelRigged, "Assets/Models/Rigged/Shadii_Animated2.wwm");

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
	testAnimator.AddAnimation(animation, 0, 1, 1);
	testAnimator.AddAnimation(animation2, 0, 1, 0.2f);
	testAnimator.AddAnimation( animation3, 0, 1, 0.0f );

	float rotationY = cs::c_pi * 1.0f;
	mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
	mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
	
	dx::SimpleMath::Vector2 posTest;
	posTest.x = 100;
	posTest.y = 70;
	const wchar_t* inputText = L"Shadii";
	cs::Color4f color(0.034f, 0.255f, 0.0f, 1.0f);
	shared_ptr<TextRenderable> text = Renderer::CreateTextRenderable(inputText, posTest, FontDefault, color);

	int frames = 0;
	cs::Timer deltaTimer;

	for (bool running = true; running; frames++)
	{
		m_debug->ClearFrameTrace();
		running = !m_renderer->UpdateWindow();

		float dTime = deltaTimer.Lap();
		m_debug->CalculateFps(dTime);

		static float dTimeAcc = 0.0f;
		dTimeAcc += dTime;

		Move(dTime);

		testAnimator.Update(dTime);

		m_game->Update();
		m_sound->Update();
		rotationY += 2 * dTime;
		mesh->worldMatrix = Mat::translation3(0, -0.8f, 1) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
		mesh2->worldMatrix = Mat::translation3(0, -0.8f, 5) * Mat::rotation3(cs::c_pi * -0.5f, -rotationY, 0); // cs::c_pi * 0.9f

		m_renderer->Draw();
		m_renderer->BeginGui();

		if (ImGui::Begin( "Animation" ))
		{
			ImGui::SliderFloat( "walk influence", &testAnimator.loadedAnimations[1].influence, 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat( "crouch influence", &testAnimator.loadedAnimations[2].influence, 0.0f, 1.0f, "ratio = %.3f" );
		}
		ImGui::End();


		m_debug->DrawConsole();
		m_renderer->EndGui();
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
	Camera& camera = Renderer::GetCamera();

	Vec3 movement = Vec3(0, 0, 0);

	if (Input::Get().IsKeybindDown(KeybindForward))		movement.z += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindBackward))	movement.z -= 1.0f;
	if (Input::Get().IsKeybindDown(KeybindRight))		movement.x += 1.0f;
	if (Input::Get().IsKeybindDown(KeybindLeft))		movement.x -= 1.0f;

	camera.SetPosition(camera.GetPosition() + movement * dTime);
	camera.Update();
}
