#include "Core.h"
#include "Whisperwoods.h"

#include "Game.h"
#include "Input.h"
#include "AudioSource.h"
#include "FBXImporter.h"
#include "Animator.h"
#include <imgui.h>
#include "TextRenderable.h"

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
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 0.5f, 0, 10, soundPtr);
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

	//// Static Models
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
	//BuildWWM( "Assets/Models/FBX/Static/BananaPlant.fbx", false );

	//// Animations
	//BuildWWA( "Assets/Models/FBX/Rigged/Grafiki_Animations.fbx" );
	//BuildWWA( "Assets/Models/FBX/Rigged/Shadii_Animations.fbx" );
	//BuildWWA( "Assets/Models/FBX/Rigged/Carcinian_Animations.fbx" );

	cs::List<VertexTextured> planeVerts = { 
		VertexTextured({-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f,0.0f,0.0f, 0.0f}), 
		VertexTextured({ 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f, 0.0f}),
		VertexTextured({-0.5f, 0.0f,-0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f,1.0f,0.0f, 0.0f}),
		VertexTextured({ 0.5f, 0.0f,-0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f,1.0f,0.0f, 0.0f})
	};
	cs::List<int> planeIndicies = { 0,1,3,0,3,2 };
	BuildWWM(planeVerts, planeIndicies, "room_plane");

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

	m_levelHandler = std::make_unique<LevelHandler>();
	m_levelHandler->LoadFloors();
}

Whisperwoods::~Whisperwoods()
{
}


//Mat4 ConvertToMat4(DirectX::XMFLOAT4X4* mat)
//{
//	Mat4 returnMat;
//	returnMat(0, 0) = mat->_11;
//	returnMat(0, 1) = mat->_12;
//	returnMat(0, 2) = mat->_13;
//	returnMat(0, 3) = mat->_14;
//	returnMat(1, 0) = mat->_21;
//	returnMat(1, 1) = mat->_22;
//	returnMat(1, 2) = mat->_23;
//	returnMat(1, 3) = mat->_24;
//	returnMat(2, 0) = mat->_31;
//	returnMat(2, 1) = mat->_32;
//	returnMat(2, 2) = mat->_33;
//	returnMat(2, 3) = mat->_34;
//	returnMat(3, 0) = mat->_41;
//	returnMat(3, 1) = mat->_42;
//	returnMat(3, 2) = mat->_43;
//	returnMat(3, 3) = mat->_44;
//	return returnMat;
//}


void Whisperwoods::Run()
{
	// Main frame loop

	//m_renderer->LoadLevel(&level);

	LevelFloor floor = {};
	m_levelHandler->GenerateFloor(&floor);

	Level& level = floor.rooms[0];
	Renderer::LoadEnvironment(&level);

	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");

	m_game->InitGame(m_renderer.get());


	int frames = 0;
	cs::Timer deltaTimer;

	
	
	Enemy PatrolEnemy("Carcinian_Animated.wwm", "Carcinian_Animations.wwa", Mat::scale3(1.25f, 1.25f, 1.25f) * Mat::translation3(0, 0, 0)* Mat::rotation3(cs::c_pi * -0.5f, 0, 0));
	for (int i = 0; i < 1/*testRoom.m_levelResource.patrolsClosed.Size()*/; i++)
	{
		for (int j = 0; j < testRoom.m_levelResource->patrolsClosed[i].controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(testRoom.m_levelResource->patrolsClosed[i].controlPoints[j]);
			Vec3 enemyPos = testRoom.bitMapToWorldPos(bitPos);
			PatrolEnemy.AddCoordinateToPatrolPath(Vec2(-enemyPos.z, enemyPos.x), true);
			Point2 test = testRoom.worldToBitmapPoint(enemyPos);
			enemyPos = Vec3(0, 0, 0);
		}
	}
	
	/*patrolEnemy.AddCoordinateToPatrolPath(Vec2(1.0f, -5.0f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.5f, -4.2f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.35f, -3.0f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.6f, -1.65f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.4f, -0.3f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.35f, 1.0f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.5f, 1.05f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.2f, 0.25f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.25f, -0.8f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.9f, -1.5f), true);
	patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.3f, -1.9f), false);*/

	//Enemy PatrolEnemy("Carcinian_Animated.wwm", "Carcinian_Animations.wwa", Mat::scale3(1.25f, 1.25f, 1.25f)* Mat::translation3(0, -0.6f, 0)* Mat::rotation3(cs::c_pi * -0.5f, 0, 0));
	//PatrolEnemy.AddCoordinateToPatrolPath(Vec2(2.0f, 2.0f), true);
	//PatrolEnemy.AddCoordinateToPatrolPath(Vec2(0.0f, 0.0f), true);*/

	Vec3 tempRot;

	for (bool running = true; running; frames++)
	{
		m_renderer->BeginGui();

		m_debug->ClearFrameTrace();
		m_input->Update();
		running = !m_renderer->UpdateWindow();

		float dTime = deltaTimer.Lap();
		m_debug->CalculateFps(dTime);

		static float dTimeAcc = 0.0f;
		dTimeAcc += dTime;
			
		Input::Get().Update();
		
		testAnimatorGrafiki.Update(dTime);

		testEmpty.Update(dTime);
		testRoom.Update(dTime);
		PatrolEnemy.Update(dTime);
		testPlayer.Update(dTime);
		//patrolEnemy.Update(dTime);

		m_game->Update();
		m_sound->Update();
		PatrolEnemy.SeesPlayer(Vec2(testPlayer.transform.worldPosition.x, testPlayer.transform.worldPosition.z), testSource, testRoom);
		
		// Draw step
		Renderer::SetPlayerMatrix(testPlayer.transform.worldMatrix);
		m_renderer->Draw();

		//#ifdef WW_DEBUG
		m_renderer->BeginGui();
		Move(dTime, m_game->GetPlayer());

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



void Whisperwoods::Move(float dTime, Player* player)
{
	static bool cameraLock = false;
	static bool cameraPlayer = true;

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

	if (Input::Get().IsDXKeyPressed(DXKey::P))
	{
		cameraPlayer = !cameraPlayer;
		player->cameraIsLocked = cameraPlayer;
	}

	if (Input::Get().IsKeybindDown(KeybindSprint))
	{
		movement *= 5.0f;
	}

	MouseState mouseState = Input::Get().GetMouseState();

	static Vec3 rotationVec = {};
	if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
	{
		cs::Vec3 delta = Vec3((float)mouseState.y, (float)mouseState.x, 0.0f);

		rotationVec -= delta * dTime * 4.0f;
		if (!cameraPlayer)
		{
			camera.SetRotation(Quaternion::GetEuler(rotationVec));
		}
		//camera.SetRotation(Quaternion::GetEuler(rotationVec));
	}

	Input::Get().SetMode(cameraLock ? dx::Mouse::MODE_RELATIVE : dx::Mouse::MODE_ABSOLUTE);

	if (!cameraPlayer)
	{
		camera.SetPosition(camera.GetPosition() + movement * dTime);
	}
	else
	{
		Vec3 cameraCurrentPos = camera.GetPosition();
		Vec3 cameraTargetPos = player->cameraFollowTarget;
		Vec3 lerped = Lerp(cameraCurrentPos, cameraTargetPos, dTime * 5);
		camera.SetPosition(lerped);

		// same as in player only using the bigass function above.
		Vec3 direction = player->transform.position - cameraTargetPos;
		direction.Normalize();
		Quaternion cameraCurrentRot = camera.GetRotation();
		//Quaternion cameraTargetRot = Quaternion::GetDirection(direction, Vec3(0,1,0));
		Vec3 upVector(0.0f, 1.0f, 0.0f);
		Quaternion cameraTargetRot = QuaternionLookRotation(direction, upVector);

		Quaternion conj1 = cameraTargetRot.Conjugate();
		Quaternion conj2 = player->cameraLookRotationTarget.Conjugate();
		if (ImGui::Begin("Camera rotation player"))
		{
			ImGui::Text("Dir: %f, %f, %f", direction.x, direction.y, direction.z);
			ImGui::Text("Rot: %f, %f, %f, %f", conj1.x, conj1.y, conj1.z, conj1.w);
			ImGui::Text("RotP: %f, %f, %f, %f", conj2.x, conj2.y, conj2.z, conj2.w);
			ImGui::DragFloat( "Camera Follow Distance", &player->cameraFollowDistance, 0.05f, 0.1f, 10.0f );
			ImGui::DragFloat( "Camera Follow Tilt", &player->cameraFollowTilt, 0.05f, 0.1f, cs::c_pi/2-0.1f );
			ImGui::DragFloat3( "Camera lookAt offset", (float*)&player->cameraLookTargetOffset, 0.1f );
		}
		ImGui::End();

		//Quaternion slerped = Quaternion::GetSlerp(cameraCurrentRot, cameraTargetRot.Conjugate(), dTime * 5);
		//Quaternion slerped = Lerp((cameraCurrentRot.x != NAN) ? cameraCurrentRot : conj2, conj2, dTime * 5); // sometimes turns black
		//Quaternion slerped = Lerp(cameraCurrentRot, conj2, dTime * 5); // sometimes turns black

		// sometimes the lerping/target goes bad so this is required or screen can go black.
		if (std::isnan( cameraCurrentRot.x ) || std::isnan( cameraCurrentRot.y ) || std::isnan( cameraCurrentRot.z ) || std::isnan( cameraCurrentRot.w ))
		{
			camera.SetRotation( conj2 );
		}
		else
		{
			Quaternion slerped;
			if (!(std::isnan(conj2.x) || std::isnan(conj2.y) || std::isnan(conj2.z) || std::isnan(conj2.w)))
			{
				slerped = Lerp( cameraCurrentRot, conj2, dTime * 5 );
				slerped.NormalizeThis();
			}

			//LOG_TRACE("Rot from: %f, %f, %f, %f", cameraCurrentRot.x, cameraCurrentRot.y, cameraCurrentRot.z, cameraCurrentRot.w);
			//LOG_TRACE("Rot to: %f, %f, %f, %f", conj2.x, conj2.y, conj2.z, conj2.w);

			camera.SetRotation(slerped);
		}
	}
	camera.Update();
}
