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

void TestPlay(void*, void*)
{
	FMOD::Sound* soundPtr = (Resources::Get().GetSound("Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.7f, 1.0f, 0, 10, soundPtr);
	testSource.mix2d3d = 0;
	testSource.Play();
}

Whisperwoods::Whisperwoods(HINSTANCE instance)
{
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
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_1_Present.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_2_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_2_Present.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_3_Future.fbx", false);
	//BuildWWM("Assets/Models/FBX/Static/Medium_Tree_3_Present.fbx", false);
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

	BuildRoomWWM( 16, 0.5f, 10.0f, "room_walls_floor" );


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
	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");
	
	m_game->Init();
	m_game->LoadHubby();


	// Test generate a floor
	LevelFloor tempFloor;
	FloorParameters fParams;
	fParams.seed = 123;
	fParams.angleSteps = 0;
	fParams.pushSteps = 3;
	fParams.roomCount = 8;
	EnvironmentalizeParameters eParams = { 0 };
	m_game->m_levelHandler->GenerateFloor(&tempFloor, fParams, eParams );

	// Generate a image from the floor data
	shared_ptr<uint8_t> imageData = m_game->m_levelHandler->GenerateFloorImage(1024, 1024, tempFloor);
	TextureResource* newTexture = Resources::Get().CreateTexture(m_renderer->GetRenderCore(), "Test", imageData.get(), 1024 * 1024);

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



	//************* power cooldown
	testGui.AddGUIElement({ 0.475f,-0.82f }, { 0.5f,0.1f }, nullptr, nullptr);
	testGui.GetElement(3)->colorTint = Vec3(0.08f, 0.18f, 0.8f);
	testGui.GetElement(3)->alpha = 0.6f;
	testGui.GetElement(3)->vectorData = Vec3(1, 1, 1);
	testGui.GetElement(3)->floatData = 0.5f;
	testGui.GetElement(3)->intData = Point4(0, 1, 0, 0); // Makes it follow the float value.
	testGui.GetElement(3)->firstTexture = nullptr;
	testGui.GetElement(3)->secondTexture = Resources::Get().GetTexture("StaminaBarMask09.png");
	//*************

	// Test of the cutscene system.
	CutsceneController testController;
	shared_ptr<Cutscene> testCutScene(new Cutscene("Test scene"));
	testCutScene->AddChannel( std::shared_ptr<CutsceneCameraChannel>( new CutsceneCameraChannel( "Main camera", &Renderer::GetCamera())));
	testCutScene->AddChannel( std::shared_ptr<CutsceneAnimatorChannel>( new CutsceneAnimatorChannel( "Player Animator", m_game->GetPlayer()->characterAnimator.get())));
	testCutScene->AddChannel( std::shared_ptr<CutsceneTransformChannel>( new CutsceneTransformChannel( "Player Transform", &m_game->GetPlayer()->transform )));
	testController.m_cutscenes.Add( testCutScene );
	testController.ActivateCutscene( 0 );

	CutsceneCameraChannel* channel = (CutsceneCameraChannel*)testController.m_cutscenes[0]->channels[0].get();
	channel->AddKey(shared_ptr<CutsceneCameraKey>(new CutsceneCameraKey(0.1f, {0,0,0}, Quaternion::GetEuler( {0,0,0} ), 90, 1)));
	//channel->keys[0]->frame = 0;

	channel->AddKey(shared_ptr<CutsceneCameraKey>(new CutsceneCameraKey(0.1f, { 0,5,0 }, Quaternion::GetEuler( { 0,0,0 } ), 90, 1)));
	//channel->keys[1]->frame = 1;

	channel->AddKey(shared_ptr<CutsceneCameraKey>(new CutsceneCameraKey(0.1f, { 0,1,5 }, Quaternion::GetEuler( { 0,0,0 } ), 90, 1)));
	//channel->keys[2]->frame = 2;

	//testCutScene.AddKey( std::shared_ptr< CutsceneTransformKey >(new CutsceneTransformKey( 0.5f, m_game->GetPlayer(), {0,0,0}, Quaternion::GetEuler({0,0,0}), {1,1,1})));

	// Main frame loop
	int frames = 0;
	cs::Timer deltaTimer;
	for (bool running = true; running; frames++)
	{
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
		
		// Test of cinematics
		testController.Update();
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


		if (Input::Get().IsDXKeyPressed( DXKey::B ))
		{
			Debug::ExecuteCommand( "Duck", "play" );
			targetAlpha = !targetAlpha;
		}
		testGui.GetElement( 2 )->alpha = LerpFloat( testGui.GetElement( 2 )->alpha, targetAlpha, 4.0f * dTime );


		// Button/Interaction Test
		//if (Input::Get().GetMouseState().leftButton && !Input::Get().GetLastMouseState().leftButton)
		//{
		//	//bool isInside = testGui.GetElement( 2 )->TestMouse();
		//	//LOG_TRACE( "Inside: %d ", isInside );
		//	if (testGui.GetElement( 2 )->TestMouse())
		//	{
		//		Debug::ExecuteCommand( "Duck", "play" );
		//		targetAlpha = !targetAlpha;
		//	}
		//}
		//else if (!Input::Get().GetMouseState().leftButton)
		//{
		//	if( testGui.GetElement( 2 )->TestMouse() )
		//	{
		//		testGui.GetElement( 2 )->colorTint = Vec3( 0.5, 1.0f, 0.5 );
		//	}
		//	else
		//	{
		//		testGui.GetElement( 2 )->colorTint = Vec3( 1, 1, 1 );
		//	}
		//}

		if (ImGui::Begin( "Shadow PS Test" ))
		{
			ImGui::Checkbox( "Use shadow PS", &m_renderer->GetRenderCore()->m_bindShadowPS );
		}
		ImGui::End();

		// Main game update
		m_game->Update(dTime, m_renderer.get());

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

		// Camera update
		Move(dTime, m_game->GetPlayer(), &testController);

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

Vec3 Lerp(Vec3 a, Vec3 b, float t)
{
	return a * (1.0f - t) + b * t;
}

Quaternion QuaternionLookRotation(Vec3 forward, Vec3 up)
{
	forward.Normalize();

	Vec3 vector = forward.Normalize();
	Vec3 vector2 = up.Cross(vector).Normalize();
	Vec3 vector3 = vector.Cross(vector2);
	float m00 = vector2.x;
	float m01 = vector2.y;
	float m02 = vector2.z;
	float m10 = vector3.x;
	float m11 = vector3.y;
	float m12 = vector3.z;
	float m20 = vector.x;
	float m21 = vector.y;
	float m22 = vector.z;


	float num8 = (m00 + m11) + m22;
	Quaternion quaternion;
	if (num8 > 0.0f)
	{
		float num = (float)std::sqrt(num8 + 1.0f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)std::sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		float num6 = (float)std::sqrt(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	float num5 = (float)std::sqrt(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

Quaternion Lerp(Quaternion q0, Quaternion q1, float t)
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet((float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w);
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet((float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w);
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSlerp(Q0, Q1, t);
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4(&FL4, OUTPUT);
	return Quaternion(FL4.x, FL4.y, FL4.z, FL4.w);
}

Vec3 EulerAngles( Quaternion q )
{
	Vec3 returnValue;
	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	returnValue.x = std::atan2f( sinr_cosp, cosr_cosp );

	// pitch (z-axis rotation)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs( sinp ) >= 1)
		returnValue.y = std::copysignf( DirectX::XM_PI / 2, sinp ); // use 90 degrees if out of range
	else
		returnValue.y = std::asinf( sinp );

	// yaw (y-axis rotation)
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	returnValue.z = std::atan2f( siny_cosp, cosy_cosp );

	return returnValue;
}

void Whisperwoods::Move(float dTime, Player* player, CutsceneController* cutSceneController)
{
	static bool cameraLock = false;
	static bool cameraPlayer = true;
	Camera& camera = Renderer::GetCamera();

	if (!cutSceneController->CutsceneActive())
	{
		if (Input::Get().IsDXKeyPressed(DXKey::R))
		{
			cameraLock = !cameraLock;
		}

		if (Input::Get().IsDXKeyPressed(DXKey::P))
		{
			cameraPlayer = !cameraPlayer;
			player->cameraIsLocked = cameraPlayer;
		}


		MouseState mouseState = Input::Get().GetMouseState();
		Input::Get().SetMode(cameraLock ? dx::Mouse::MODE_RELATIVE : dx::Mouse::MODE_ABSOLUTE);
		
		if (!cameraPlayer)
		{
			// Debug Camera Movement
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
			if (Input::Get().IsKeybindDown(KeybindSprint))
			{
				movement *= 5.0f;
			}
			static Vec3 rotationVec = {};
			if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
			{
				cs::Vec3 delta = Vec3( (float)mouseState.y, (float)mouseState.x, 0.0f );
				rotationVec -= delta * dTime * 2.0f;
				camera.SetRotation( Quaternion::GetEuler( { rotationVec.x, rotationVec.y, rotationVec.z } ).Conjugate() );

				if (ImGui::Begin( "Camera rotation dev" ))
				{
					ImGui::Text( "Rot Vec: %f, %f, %f", rotationVec.x, rotationVec.y, rotationVec.z );
					ImGui::Text( "Rot: %f, %f, %f, %f", camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z, camera.GetRotation().w );
					ImGui::Text( "Dir: %f, %f, %f", camera.GetDirection().x, camera.GetDirection().y, camera.GetDirection().z );
					ImGui::Text( "Delta: %f, %f, %f", delta.x, delta.y, delta.z );

				}
				ImGui::End();
				//camera.SetRotation(Quaternion::GetEuler(rotationVec));
			}
			camera.SetPosition(camera.GetPosition() + movement * dTime);
		}
		else
		{
			Vec3 cameraCurrentPos = camera.GetPosition();
			Vec3 cameraTargetPos = player->cameraFollowTarget;
			Quaternion cameraCurrentRot = camera.GetRotation();
			Quaternion cameraTargetRot = player->cameraLookRotationTarget;

			Vec3 lerped = Lerp(cameraCurrentPos, cameraTargetPos, dTime * 5);
			camera.SetPosition(lerped);
			if (!(std::isnan( cameraTargetRot.x) || std::isnan( cameraTargetRot.y) || std::isnan( cameraTargetRot.z) || std::isnan( cameraTargetRot.w)))
			{
				Quaternion slerped;
				slerped = Lerp(cameraCurrentRot, cameraTargetRot, cs::fclamp( dTime * 5.0f, 0.0001f, 1.0f ) );
				slerped.NormalizeThis();
				camera.SetRotation(slerped);
			}
			if (ImGui::Begin("Camera rotation player"))
			{
				Vec3 playPos = player->transform.GetWorldPosition();
				Quaternion playerRot = player->transform.GetWorldRotation();
				Vec3 playerRotEuler = EulerAngles( playerRot );
				ImGui::Text( "Player Pos: %f, %f, %f", playPos.x, playPos.y, playPos.z );
				ImGui::Text( "Player Rot: %f, %f, %f, %f", playerRot.x, playerRot.y, playerRot.z, playerRot.w );
				ImGui::Text( "Player Rot Euler deg: %f, %f, %f", playerRotEuler.x * RAD2DEG, playerRotEuler.y * RAD2DEG, playerRotEuler.z * RAD2DEG );
				//ImGui::Text("Dir: %f, %f, %f", direction.x, direction.y, direction.z);
				ImGui::Text("RotP: %f, %f, %f, %f", cameraTargetRot.x, cameraTargetRot.y, cameraTargetRot.z, cameraTargetRot.w);
				ImGui::DragFloat("Camera Follow Distance", &player->cameraFollowDistance, 0.05f, 0.1f, 10.0f);
				ImGui::DragFloat("Camera Follow Tilt", &player->cameraFollowTilt, 0.05f, 0.1f, cs::c_pi / 2 - 0.1f);
				ImGui::DragFloat3("Camera lookAt offset", (float*)&player->cameraLookTargetOffset, 0.1f);
			}
			ImGui::End();
		}
	}

	camera.Update();
}
