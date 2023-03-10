#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"
#include "Animator.h"
#include <imgui.h>
#include "TextRenderable.h"
#include "GUI.h"
#include "TextureResource.h"

#include "CutsceneController.h"

#include "Player.h"
#include "Empty.h"
#include "StaticObject.h"
#include "Room.h"


#include "LevelImporter.h"
#include "WWMBuilder.h"
#include "WWABuilder.h"

// TODO: Dudd include. Only used for getting test sound.
#include "SoundResource.h"

Whisperwoods* Whisperwoods::s_whisperwoods = nullptr;

//void TestPlay(void*, void*)
//{
//	FMOD::Sound* soundPtr = (Resources::Get().GetSound("Duck.mp3"))->currentSound;
//	AudioSource testSource(Vec3(0, 0, 0), 0.7f, 1.0f, 0, 10, soundPtr);
//	testSource.mix2d3d = 0;
//	testSource.Play();
//}

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
	s_whisperwoods = this;

	m_debug = std::make_unique<Debug>();			// Debug initialization should be kept first
	m_debug->CaptureStreams(true, true, true);

	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	//// WWM Building below
	//// Rigged Models
	//BuildWWM( "Assets/Models/FBX/Rigged/Carcinian_Animations.fbx", true );
	//BuildWWM( "Assets/Models/FBX/Rigged/Grafiki_Animated.fbx", true );
	//BuildWWM( "Assets/Models/FBX/Rigged/Shadii_Animated.fbx", true );
	//BuildWWM( "Assets/Models/FBX/Rigged/Shadii_Animated2.fbx", true );
	//BuildWWM( "Assets/Models/FBX/Rigged/Shadii_Rigged_Optimized.fbx", true );

	////// Static Models
	//BuildWWM( "Assets/Models/FBX/Static/EssenseBloom.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/Ground.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/BigTrees.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/BigPlants.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/SmallPlants.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/MediumTrees.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/Stones.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/Grafitree.fbx", false );
	//BuildWWM( "Assets/Models/FBX/Static/MediumTrees.fbx", false );
	//BuildWWM("Assets/Models/FBX/Static/Big_Trunk_1.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Big_Trunk_2.fbx", false);
	//BuildWWM( "Assets/Models/FBX/Static/BananaPlant.fbx", false, 1.0f );
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_1_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_1_Present.fbx", false, 1.5f);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_2_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_2_Present.fbx", false, 1.5f );
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_3_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_3_Present.fbx", false, 1.5f );
	//BuildWWM("Assets/Models/FBX/Static/Stone_1_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Stone_1_Present.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Stone_2_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Stone_2_Present.fbx", false);

	//// Animations
	//BuildWWA( "Assets/Models/FBX/Rigged/Grafiki_Animations.fbx" );
	//BuildWWA( "Assets/Models/FBX/Rigged/Shadii_Animations.fbx" );
	//BuildWWA( "Assets/Models/FBX/Rigged/Carcinian_Animations.fbx" );

	//cs::List<VertexTextured> planeVerts = { 
	//	VertexTextured({ 0.5f, 0.0f,-0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f,0.0f,0.0f, 0.0f}), 
	//	VertexTextured({-0.5f, 0.0f,-0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f, 0.0f}),
	//	VertexTextured({ 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f,1.0f,0.0f, 0.0f}),
	//	VertexTextured({-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f,1.0f,0.0f, 0.0f})
	//};
	//cs::List<int> planeIndicies = { 0,1,3,0,3,2 };
	//BuildWWM(planeVerts, planeIndicies, "room_plane");


	//cs::List<VertexTextured> rectVerts = { 
	//	VertexTextured({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }), 
	//	VertexTextured({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }),
	//	VertexTextured({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }),
	//	VertexTextured({ 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 0.0f })
	//};

	//cs::List<int> rectIndicies = { 0,1,2,3,2,1 };
	//BuildWWM(rectVerts, rectIndicies, "ui_rect");

	//BuildRoomWWM( 16, 0.5f, 10.0f, "room_walls_floor" );


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

	m_renderer->SetupEnvironmentAssets();
}

Whisperwoods::~Whisperwoods()
{
}

float LerpFloat( float a, float b, float t )
{
	return a * (1.0f - t) + b * t;
}


void Whisperwoods::Run()
{
	//Debug::RegisterCommand(TestPlay, "play", "Play a quack.");
	Debug::RegisterCommand(GodMode, "godmode", "Enable god mode.");
	Debug::RegisterCommand(PeasantMode, "peasantmode", "Disable god mode.");
	
	m_game->Init();
	m_game->LoadHubby();

	// Test GUI
	// Stamina bar
	GUI testGui;
	testGui.AddGUIElement({ 0.28f,-0.90f }, { 0.7f,0.08f }, nullptr, nullptr);
	testGui.GetElement( 0 )->colorTint = Vec3(1.0f, 0.72f, 0.0f);
	testGui.GetElement( 0 )->alpha = 0.6f;
	testGui.GetElement( 0 )->vectorData = Vec3( 1, 1, 1 );
	testGui.GetElement( 0 )->floatData = 0.5f;
	testGui.GetElement( 0 )->intData = Point4( 0, 1, 0, 0 ); // Makes it follow the float value.
	testGui.GetElement( 0 )->firstTexture = nullptr;
	testGui.GetElement( 0 )->secondTexture = Resources::Get().GetTexture("StaminaBarMask09.png");

	// Minimap
	testGui.AddGUIElement({ -1.0f,0.3f }, { 0.4f*0.9f,0.4f*1.6f }, nullptr, nullptr);
	testGui.GetElement( 1 )->colorTint = Vec3(1, 1, 1);
	testGui.GetElement( 1 )->alpha = 0.6f;
	testGui.GetElement( 1 )->intData = Point4( 1, 0, 0, 0 ); // makes it transform with the playermatrix
	testGui.GetElement( 1 )->firstTexture = Resources::Get().GetTexture( "Hubby.png" );
	testGui.GetElement( 1 )->secondTexture = Resources::Get().GetTexture("HudMask2.png");

	// Duck
	testGui.AddGUIElement( { -0.5f,-0.5f }, { 1.0f,1.0f }, nullptr, nullptr );
	testGui.GetElement( 2 )->colorTint = Vec3( 1, 1, 1 );
	testGui.GetElement( 2 )->alpha = 1.0f;
	testGui.GetElement( 2 )->intData = Point4( 0, 0, 0, 0 ); // No special flags, just the image
	testGui.GetElement( 2 )->firstTexture = Resources::Get().GetTexture( "duck.jpg" );
	testGui.GetElement( 2 )->secondTexture = Resources::Get().GetTexture( "StaminaBarMask09.png" );
	float targetAlpha = 0.0f;

	// Power cooldown
	testGui.AddGUIElement({ 0.475f,-0.82f }, { 0.5f,0.1f }, nullptr, nullptr);
	testGui.GetElement(3)->colorTint = Vec3(0.08f, 0.18f, 0.8f);
	testGui.GetElement(3)->alpha = 0.6f;
	testGui.GetElement(3)->vectorData = Vec3(1, 1, 1);
	testGui.GetElement(3)->floatData = 0.5f;
	testGui.GetElement(3)->intData = Point4(0, 1, 0, 0); // Makes it follow the float value.
	testGui.GetElement(3)->firstTexture = nullptr;
	testGui.GetElement(3)->secondTexture = Resources::Get().GetTexture("StaminaBarMask09.png");

	// time for tutorial text. Change alpha to make them active or not
	float textAlpha = 0.7f;

	//tutorial 1
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(4)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(4)->alpha = textAlpha;
	testGui.GetElement(4)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(4)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(4)->secondTexture = Resources::Get().GetTexture("tut1Text.png");

	//tutorial 2
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(5)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(5)->alpha = textAlpha;
	testGui.GetElement(5)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(5)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(5)->secondTexture = Resources::Get().GetTexture("tut2Text.png");

	//tutorial 3
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(6)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(6)->alpha = textAlpha;
	testGui.GetElement(6)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(6)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(6)->secondTexture = Resources::Get().GetTexture("tut3Text.png");

	//tutorial 4
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(7)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(7)->alpha = textAlpha;
	testGui.GetElement(7)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(7)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(7)->secondTexture = Resources::Get().GetTexture("tut4Text.png");

	//tutorial 5
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(8)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(8)->alpha = textAlpha;
	testGui.GetElement(8)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(8)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(8)->secondTexture = Resources::Get().GetTexture("tut5Text.png");

	//tutorial 6 present
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(9)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(9)->alpha = textAlpha;
	testGui.GetElement(9)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(9)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(9)->secondTexture = Resources::Get().GetTexture("tut61Text.png");

	//tutorial 6 future
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(10)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(10)->alpha = textAlpha;
	testGui.GetElement(10)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(10)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(10)->secondTexture = Resources::Get().GetTexture("tut62Text.png");

	//tutorial 7
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(11)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(11)->alpha = textAlpha;
	testGui.GetElement(11)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(11)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(11)->secondTexture = Resources::Get().GetTexture("tut7Text.png");


	//get da bloom
	testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	testGui.GetElement(12)->colorTint = Vec3(1, 1, 1);
	testGui.GetElement(12)->alpha = textAlpha;
	testGui.GetElement(12)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	testGui.GetElement(12)->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	testGui.GetElement(12)->secondTexture = Resources::Get().GetTexture("getDaBloom.png");

	// Cutscene element 1
	testGui.AddGUIElement( { -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr );
	testGui.GetElement( 13 )->colorTint = Vec3( 0, 0, 0 );
	testGui.GetElement( 13 )->alpha = 0.0;
	testGui.GetElement( 13 )->intData = Point4( 0, 0, 0, 0 ); // No special flags, just the image
	testGui.GetElement( 13 )->firstTexture = Resources::Get().GetTexture( "TextWhite.png" );

	// Cutscene element 2
	testGui.AddGUIElement( { -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr );
	testGui.GetElement( 14 )->colorTint = Vec3( 0, 0, 0 );
	testGui.GetElement( 14 )->alpha = 0.0;
	testGui.GetElement( 14 )->intData = Point4( 0, 0, 0, 0 ); // No special flags, just the image
	testGui.GetElement( 14 )->firstTexture = Resources::Get().GetTexture( "TextWhite.png" );
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo1.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo2.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo3.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo4.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo5.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo6.png"));
	testGui.GetElement(14)->alternativeImages.Add(Resources::Get().GetTexture("convo7.png"));




	//testGui.GetElement( 13 )->secondTexture = Resources::Get().GetTexture( "tut1Text.png" );

	//// loading screen
	//testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	//testGui.GetElement(13)->colorTint = Vec3(1, 1, 1);
	//testGui.GetElement(13)->alpha = 0.0f;
	//testGui.GetElement(13)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	//testGui.GetElement(13)->firstTexture = Resources::Get().GetTexture("loadingScreen.png");

	m_game->SetGUI(&testGui);
	m_game->InitCutscene();


	// Main frame loop
	int frames = 0;
	cs::Timer deltaTimer;
	for (bool running = true; running; frames++)
	{

		if (m_game->youWin || m_game->IsInHubby())
		{
			testGui.GetElement(0)->uiRenderable->enabled = false;
			testGui.GetElement(1)->uiRenderable->enabled = false;
			testGui.GetElement(3)->uiRenderable->enabled = false;
		}
		else
		{
			testGui.GetElement(0)->uiRenderable->enabled = true;
			testGui.GetElement(1)->uiRenderable->enabled = true;
			testGui.GetElement(3)->uiRenderable->enabled = true;
		}
		// Init frame
		m_renderer->BeginGui();
		m_debug->ClearFrameTrace();

		// Update inputs
		m_input->Update();
		running = !m_renderer->UpdateWindow();

		// FPS calculation
		float dTime = deltaTimer.Lap();
		m_debug->CalculateFps(dTime);
		static float dTimeAcc = 0.0f;
		dTimeAcc += dTime;

		// Update the test gui with the stamina.
		testGui.GetElement( 0 )->floatData = m_game->GetPlayer()->GetCurrentStamina()/10.0f;
		if (m_game->GetMaxStamina() == 1.0f)
		{
			testGui.GetElement(0)->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}
		else
		{
			testGui.GetElement(0)->colorTint = Vec3(1.0f, 0.72f, 0.0f);
		}

		//update test gui with power cooldown
		if (m_game->GetPlayer()->playerInFuture == false && m_game->GetPowerCooldown() != 0)
		{
			testGui.GetElement(3)->floatData = (m_game->GetMaxPowerCooldown() - m_game->GetPowerCooldown()) / m_game->GetMaxPowerCooldown();
			testGui.GetElement(3)->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}
		else if (m_game->GetPlayer()->playerInFuture == false && m_game->GetPowerCooldown() == 0)
		{
			testGui.GetElement(3)->colorTint = Vec3(0.08f, 0.18f, 0.8f);
			testGui.GetElement(3)->floatData = (m_game->GetMaxPowerCooldown() - m_game->GetPowerCooldown()) / m_game->GetMaxPowerCooldown();
		}
		else
		{
			testGui.GetElement(3)->floatData = 0.06f;
			testGui.GetElement(3)->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}

		if (m_game->showTextForPickupBloom)
		{
			testGui.GetElement(12)->uiRenderable->enabled = true;
		}
		else
		{
			testGui.GetElement(12)->uiRenderable->enabled = false;
		}

		if (Input::Get().IsDXKeyPressed( DXKey::B ))
		{
			//Debug::ExecuteCommand( "Duck", "play" );
			targetAlpha = !targetAlpha;
		}
		testGui.GetElement( 2 )->alpha = LerpFloat( testGui.GetElement( 2 )->alpha, targetAlpha, 4.0f * dTime );

		if (testGui.GetElement(2)->alpha = 0.0f)
		{
			testGui.GetElement(2)->uiRenderable->enabled = false;
		}
		else
		{
			testGui.GetElement(2)->uiRenderable->enabled = true;
		}



		if (ImGui::Begin( "Shadow PS Test" ))
		{
			ImGui::Checkbox( "Use shadow PS", &m_renderer->GetRenderCore()->m_bindShadowPS );
		}
		ImGui::End();

		// Main game update
		m_game->Update(dTime, m_renderer.get());



		// tutorial text reset
		for (int i = 4; i <= 11; i++)
		{
			testGui.GetElement(i)->uiRenderable->enabled = false;
		}

		//set active tutorial text
		if (m_game->tutorial)
		{
			if (m_game->activeTutorialLevel < 6)
			{
				testGui.GetElement(m_game->activeTutorialLevel + 3)->uiRenderable->enabled = true;
			}
			else if (m_game->activeTutorialLevel == 6)
			{
				if (m_game->GetPlayer()->playerInFuture)
				{
					testGui.GetElement(10)->uiRenderable->enabled = true;
				}
				else
				{
					testGui.GetElement(9)->uiRenderable->enabled = true;
				}
			}
			else if (m_game->activeTutorialLevel == 7)
			{
				testGui.GetElement(11)->uiRenderable->enabled = true;
			}
		}



		// Audio listener calculation (maybe move this somewhere more appropriate)
		Camera& camera = Renderer::GetCamera();
		Quaternion rotation = camera.GetRotation();
		rotation = rotation;
		Vec3 forward( 0, 0, 1 );
		forward = rotation * forward;
		forward.Normalize();
		Vec3 up( 0, 1, 0 );
		up = rotation * up;
		up.Normalize();
		Vec3 cPos = camera.GetPosition();
		FMOD_VECTOR listenerPos = { cPos.x, cPos.y, cPos.z };
		FMOD_VECTOR listenerForward = { forward.x,  forward.y,  forward.z };
		FMOD_VECTOR listenerVelocity = {0,0,0};
		FMOD_VECTOR listenerUp = { up.x, up.y, up.z };
		// Update the audio system
		m_sound->Update( listenerPos, listenerVelocity, listenerForward, listenerUp );
		
		// Draw step
		m_renderer->Draw();

		// Draw console
		m_debug->DrawConsole();

		// Profiling update
		m_renderer->UpdateGPUProfiler();

		// Wrap up and present
		m_renderer->EndGui();
		m_renderer->Present();	
	}

	m_game->DeInit();
}

void Whisperwoods::GodMode(void*, void*)
{
	s_whisperwoods->m_game->GodMode(true);
}

void Whisperwoods::PeasantMode(void*, void*)
{
	s_whisperwoods->m_game->GodMode(false);
}
