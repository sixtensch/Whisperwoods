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
	std::shared_ptr<GUIElement> staminaBarGUI = testGui.AddGUIElement({ 0.28f,-0.90f }, { 0.7f,0.08f }, nullptr, nullptr);
	staminaBarGUI->colorTint = Vec3(1.0f, 0.72f, 0.0f);
	staminaBarGUI->alpha = 0.6f;
	staminaBarGUI->vectorData = Vec3( 1, 1, 1 );
	staminaBarGUI->floatData = 0.5f;
	staminaBarGUI->intData = Point4( 0, 1, 0, 0 ); // Makes it follow the float value.
	staminaBarGUI->firstTexture = nullptr;
	staminaBarGUI->secondTexture = Resources::Get().GetTexture("StaminaBarMask09.png");

	// Duck
	std::shared_ptr<GUIElement> duckGUI = testGui.AddGUIElement( { -0.5f,-0.5f }, { 1.0f,1.0f }, nullptr, nullptr );
	duckGUI->colorTint = Vec3( 1, 1, 1 );
	duckGUI->alpha = 1.0f;
	duckGUI->intData = Point4( 0, 0, 0, 0 ); // No special flags, just the image
	duckGUI->firstTexture = Resources::Get().GetTexture( "duck.jpg" );
	duckGUI->secondTexture = Resources::Get().GetTexture( "StaminaBarMask09.png" );
	float targetAlpha = 0.0f;


	//************* power cooldown
	std::shared_ptr<GUIElement> powerCooldownGUI = testGui.AddGUIElement({ 0.475f,-0.82f }, { 0.5f,0.1f }, nullptr, nullptr);
	powerCooldownGUI->colorTint = Vec3(0.08f, 0.18f, 0.8f);
	powerCooldownGUI->alpha = 0.6f;
	powerCooldownGUI->vectorData = Vec3(1, 1, 1);
	powerCooldownGUI->floatData = 0.5f;
	powerCooldownGUI->intData = Point4(0, 1, 0, 0); // Makes it follow the float value.
	powerCooldownGUI->firstTexture = nullptr;
	powerCooldownGUI->secondTexture = Resources::Get().GetTexture("StaminaBarMask09.png");
	//**********

	// time for tutorial text. Change alpha to make them active or not
	float textAlpha = 0.7f;

	cs::List<shared_ptr<GUIElement>> tutorialTextGUIElements = {};
	
	//tutorial 1
	std::shared_ptr<GUIElement> tutorialText1GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText1GUI->colorTint = Vec3(1, 1, 1);
	tutorialText1GUI->alpha = textAlpha;
	tutorialText1GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText1GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText1GUI->secondTexture = Resources::Get().GetTexture("tut1Text.png");
	tutorialTextGUIElements.Add(tutorialText1GUI);

	//tutorial 2
	std::shared_ptr<GUIElement> tutorialText2GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText2GUI->colorTint = Vec3(1, 1, 1);
	tutorialText2GUI->alpha = textAlpha;
	tutorialText2GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText2GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText2GUI->secondTexture = Resources::Get().GetTexture("tut2Text.png");
	tutorialTextGUIElements.Add(tutorialText2GUI);

	//tutorial 3
	std::shared_ptr<GUIElement> tutorialText3GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText3GUI->colorTint = Vec3(1, 1, 1);
	tutorialText3GUI->alpha = textAlpha;
	tutorialText3GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText3GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText3GUI->secondTexture = Resources::Get().GetTexture("tut3Text.png");
	tutorialTextGUIElements.Add(tutorialText3GUI);

	//tutorial 4
	std::shared_ptr<GUIElement> tutorialText4GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText4GUI->colorTint = Vec3(1, 1, 1);
	tutorialText4GUI->alpha = textAlpha;
	tutorialText4GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText4GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText4GUI->secondTexture = Resources::Get().GetTexture("tut4Text.png");
	tutorialTextGUIElements.Add(tutorialText4GUI);

	//tutorial 5
	std::shared_ptr<GUIElement> tutorialText5GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText5GUI->colorTint = Vec3(1, 1, 1);
	tutorialText5GUI->alpha = textAlpha;
	tutorialText5GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText5GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText5GUI->secondTexture = Resources::Get().GetTexture("tut5Text.png");
	tutorialTextGUIElements.Add(tutorialText5GUI);

	//tutorial 6 present
	std::shared_ptr<GUIElement> tutorialText6PresentGUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText6PresentGUI->colorTint = Vec3(1, 1, 1);
	tutorialText6PresentGUI->alpha = textAlpha;
	tutorialText6PresentGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText6PresentGUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText6PresentGUI->secondTexture = Resources::Get().GetTexture("tut61Text.png");
	tutorialTextGUIElements.Add(tutorialText6PresentGUI);


	//tutorial 6 future
	std::shared_ptr<GUIElement> tutorialText6FutureGUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText6FutureGUI->colorTint = Vec3(1, 1, 1);
	tutorialText6FutureGUI->alpha = textAlpha;
	tutorialText6FutureGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText6FutureGUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText6FutureGUI->secondTexture = Resources::Get().GetTexture("tut62Text.png");
	tutorialTextGUIElements.Add(tutorialText6FutureGUI);

	//tutorial 7
	std::shared_ptr<GUIElement> tutorialText7GUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialText7GUI->colorTint = Vec3(1, 1, 1);
	tutorialText7GUI->alpha = textAlpha;
	tutorialText7GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialText7GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialText7GUI->secondTexture = Resources::Get().GetTexture("tut7Text.png");
	tutorialTextGUIElements.Add(tutorialText7GUI);

	//get da bloom
	std::shared_ptr<GUIElement> tutorialTextBloomGUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	tutorialTextBloomGUI->colorTint = Vec3(1, 1, 1);
	tutorialTextBloomGUI->alpha = textAlpha;
	tutorialTextBloomGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	tutorialTextBloomGUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	tutorialTextBloomGUI->secondTexture = Resources::Get().GetTexture("getDaBloom.png");
	tutorialTextGUIElements.Add(tutorialTextBloomGUI);

	// Cutscene element 1
	std::shared_ptr<GUIElement> cutsceneElement1GUI = testGui.AddGUIElement( { -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr );
	cutsceneElement1GUI->colorTint = Vec3( 0, 0, 0 );
	cutsceneElement1GUI->alpha = 0.0;
	cutsceneElement1GUI->intData = Point4( 0, 0, 0, 0 ); // No special flags, just the image
	cutsceneElement1GUI->firstTexture = Resources::Get().GetTexture( "TextWhite.png" );

	// Cutscene element 2
	std::shared_ptr<GUIElement> cutsceneElement2GUI = testGui.AddGUIElement( { -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr );
	cutsceneElement2GUI->colorTint = Vec3(0, 0, 0);
	cutsceneElement2GUI->alpha = 0.0;
	cutsceneElement2GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	cutsceneElement2GUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");

	std::shared_ptr<GUIElement> introScreenGUI = testGui.AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	introScreenGUI->colorTint = Vec3(1, 1, 1);
	introScreenGUI->alpha = 1.0f;
	introScreenGUI->uiRenderable->enabled = false;
	introScreenGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	introScreenGUI->firstTexture = Resources::Get().GetTexture("introClip.png");

	m_game->SetGUI(&testGui);
	m_game->InitCutscene();


	// Main frame loop
	int frames = 0;
	cs::Timer deltaTimer;
	for (bool running = true; running; frames++)
	{

		if (m_firstIntroPic == true)
		{
			introScreenGUI->uiRenderable->enabled = true;
			if (Input::Get().IsDXKeyPressed(DXKey::Space))
			{
				introScreenGUI->uiRenderable->enabled = false;
				m_firstIntroPic = false;
			}
		}

		if (m_game->youWin || m_game->IsInHubby())
		{
			staminaBarGUI->uiRenderable->enabled = false;
			powerCooldownGUI->uiRenderable->enabled = false;
			duckGUI->uiRenderable->enabled = false;
			m_game->m_levelHandler->MinimapSetEnable(false);
		}
		else
		{
			staminaBarGUI->uiRenderable->enabled = true;
			powerCooldownGUI->uiRenderable->enabled = true;
			duckGUI->uiRenderable->enabled = true;
			m_game->m_levelHandler->MinimapSetEnable(true);
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
		staminaBarGUI->floatData = m_game->GetPlayer()->GetCurrentStamina()/10.0f;
		if (m_game->GetMaxStamina() == 1.0f)
		{
			staminaBarGUI->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}
		else
		{
			staminaBarGUI->colorTint = Vec3(1.0f, 0.72f, 0.0f);
		}

		//update test gui with power cooldown
		if (m_game->GetPlayer()->playerInFuture == false && m_game->GetPowerCooldown() != 0)
		{
			powerCooldownGUI->floatData = (m_game->GetMaxPowerCooldown() - m_game->GetPowerCooldown()) / m_game->GetMaxPowerCooldown();
			powerCooldownGUI->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}
		else if (m_game->GetPlayer()->playerInFuture == false && m_game->GetPowerCooldown() == 0)
		{
			powerCooldownGUI->colorTint = Vec3(0.08f, 0.18f, 0.8f);
			powerCooldownGUI->floatData = (m_game->GetMaxPowerCooldown() - m_game->GetPowerCooldown()) / m_game->GetMaxPowerCooldown();
		}
		else
		{
			powerCooldownGUI->floatData = 0.06f;
			powerCooldownGUI->colorTint = Vec3(0.93f, 0.0f, 0.12f);
		}

		if (m_game->showTextForPickupBloom)
		{
			tutorialTextBloomGUI->uiRenderable->enabled = true;
		}
		else
		{
			tutorialTextBloomGUI->uiRenderable->enabled = false;
		}

		if (Input::Get().IsDXKeyPressed( DXKey::B ))
		{
			//Debug::ExecuteCommand( "Duck", "play" );
			targetAlpha = !targetAlpha;
		}
		duckGUI->alpha = LerpFloat(duckGUI->alpha, targetAlpha, 4.0f * dTime );

		if (duckGUI->alpha = 0.0f)
		{
			duckGUI->uiRenderable->enabled = false;
		}
		else
		{
			duckGUI->uiRenderable->enabled = true;
		}



		if (ImGui::Begin( "Shadow PS Test" ))
		{
			ImGui::Checkbox( "Use shadow PS", &m_renderer->GetRenderCore()->m_bindShadowPS );
		}
		ImGui::End();

		// Main game update
		if (m_firstIntroPic == false)
		{
			m_game->Update(dTime, m_renderer.get());
		}

		for (shared_ptr<GUIElement> tutorialText : tutorialTextGUIElements)
		{
			tutorialText->uiRenderable->enabled = false;
		}


		//set active tutorial text
		if (m_game->tutorial)
		{
			if (m_game->activeTutorialLevel < 6)
			{
				// How does this work?
				testGui.GetElement(m_game->activeTutorialLevel + 2)->uiRenderable->enabled = true;
			}
			else if (m_game->activeTutorialLevel == 6)
			{
				if (m_game->GetPlayer()->playerInFuture)
				{
					tutorialText6FutureGUI->uiRenderable->enabled = true;
				}
				else
				{
					tutorialText6PresentGUI->uiRenderable->enabled = true;
				}
			}
			else if (m_game->activeTutorialLevel == 7)
			{
				tutorialText7GUI->uiRenderable->enabled = true;
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
