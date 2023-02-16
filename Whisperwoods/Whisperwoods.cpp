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

#include "LevelImporter.h"

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

	// WWM Building below
	
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

	//ModelRiggedResource carcinian;
	//importer.ImportFBXRigged("Assets/Carcinian_Animated.fbx", &carcinian);
	//std::string path4 = importer.SaveWMM(&carcinian, "Assets/Models/Rigged/");

	/*ModelRiggedResource grafikiAnimated;
	importer.ImportFBXRigged("Assets/Models/FBX/Rigged/Grafiki_Animated.fbx", &grafikiAnimated);
	std::string path1 = importer.SaveWMM(&grafikiAnimated, "Assets/Models/Rigged/");*/

	//importer.ImportFBXRigged("Assets/Shadii_Animations.fbx", &riggedModel);
	//path = importer.SaveWMM(&riggedModel, "Assets/Models/Rigged/");

	//ModelStaticResource staticTestModelWrite;
	//ModelStaticResource staticTestModelWrite2;
	//ModelStaticResource staticTestModelWrite3;
	//ModelStaticResource staticTestModelWrite4;
	//ModelStaticResource staticTestModelWrite5;
	//ModelStaticResource staticTestModelWrite6;
	//ModelStaticResource staticTestModelWrite7;
	//importer.ImportFBXStatic( "Assets/Models/FBX/Static/Ground.fbx", &staticTestModelWrite);
	//std::string path4 = importer.SaveWMM(&staticTestModelWrite, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/BigTrees.fbx", &staticTestModelWrite2);
	//std::string path5 = importer.SaveWMM(&staticTestModelWrite2, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/BigPlants.fbx", &staticTestModelWrite3);
	//std::string path6 = importer.SaveWMM(&staticTestModelWrite3, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/SmallPlants.fbx", &staticTestModelWrite4);
	//std::string path7 = importer.SaveWMM(&staticTestModelWrite4, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/MediumTrees.fbx", &staticTestModelWrite5);
	//std::string path8 = importer.SaveWMM(&staticTestModelWrite5, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/Stones.fbx", &staticTestModelWrite6);
	//std::string path9 = importer.SaveWMM(&staticTestModelWrite6, "Assets/Models/Static/");

	//importer.ImportFBXStatic("Assets/Models/FBX/Static/Grafitree.fbx", &staticTestModelWrite7);
	//std::string path10 = importer.SaveWMM(&staticTestModelWrite7, "Assets/Models/Static/");


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

	LevelResource level = {};
	LevelImporter::ImportImage("Examplemap.png", m_renderer->GetRenderCore(), &level);

	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	AudioSource testSource(Vec3(0, 0, 0), 0.2f, 1.1f, 0, 10, soundPtr);
	testSource.Play();

	Debug::RegisterCommand(TestPlay, "play", "Play a quack.");

	// Test meshes
	shared_ptr<MeshRenderableStatic> mesh2 = Renderer::CreateMeshStatic("ShadiiTest.wwm");
	shared_ptr<MeshRenderableRigged> grafiki = Renderer::CreateMeshRigged("Grafiki_Animated.wwm");
	
	Player testPlayer("Shadii_Animated.wwm", "Assets/Models/FBX/Rigged/Shadii_Animations.fbx", Mat::translation3(0, -0.5f, 0) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0));
	Empty testEmpty;
	testEmpty.AddChild(&testPlayer);

	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0, -5.5f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);

	Resources& resources2 = Resources::Get();

	shared_ptr<MeshRenderableStatic> ground = Renderer::CreateMeshStatic("Ground.wwm");
	ground->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	ground->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneGround.wwmt"));
	ground->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneBackground.wwmt"));

	shared_ptr<MeshRenderableStatic> bigTrees = Renderer::CreateMeshStatic("BigTrees.wwm");
	bigTrees->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	bigTrees->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneBigTree.wwmt"));

	shared_ptr<MeshRenderableStatic> bigPlants = Renderer::CreateMeshStatic("BigPlants.wwm");
	bigPlants->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	bigPlants->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneBanana.wwmt"));

	shared_ptr<MeshRenderableStatic> smallPlants = Renderer::CreateMeshStatic("SmallPlants.wwm");
	smallPlants->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	smallPlants->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneTopDownPlant.wwmt"));

	shared_ptr<MeshRenderableStatic> mediumTrees = Renderer::CreateMeshStatic("MediumTrees.wwm");
	mediumTrees->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	mediumTrees->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneMediumTree.wwmt"));

	shared_ptr<MeshRenderableStatic> stones = Renderer::CreateMeshStatic("Stones.wwm");
	stones->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	stones->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneStones.wwmt"));

	shared_ptr<MeshRenderableStatic> grafiTree = Renderer::CreateMeshStatic("Grafitree.wwm");
	grafiTree->worldMatrix = worldScale * worldPos * worldRot; // cs::c_pi * 0.9f
	grafiTree->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "TestSceneGrafitree.wwmt"));


	// Grafiki animations test
	Resources resources = Resources::Get();
	Animator testAnimatorGrafiki((ModelRiggedResource*)resources.GetResource(ResourceTypeModelRigged, "Grafiki_Animated.wwm"));
	FBXImporter importer;
	shared_ptr<AnimationResource> resource2(new AnimationResource);
	importer.ImportFBXAnimations("Assets/Models/FBX/Rigged/Grafiki_Animations.fbx", resource2.get());
	Animation* animation4 = &resource2->animations[2]; 
	Animation* animation5 = &resource2->animations[3];
	Animation* animation6 = &resource2->animations[4];
	float speed2 = 0.5f;
	testAnimatorGrafiki.AddAnimation(animation4, 0, speed2, 1.0f);
	testAnimatorGrafiki.AddAnimation(animation5, 0, speed2, 0.0f);
	testAnimatorGrafiki.AddAnimation(animation6, 0, speed2, 0.0f);

	// Test meshes transforms
	float rotationY = cs::c_pi * 1.0f;
	mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f
	grafiki->worldMatrix = Mat::translation3(2.5f, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, rotationY, 0); // cs::c_pi * 0.9f

	// Static shady materials test
	mesh2->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiBody.wwmt"));
	mesh2->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiWhite.wwmt"));
	mesh2->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiPupil.wwmt"));
	
	// Grafiki materials test
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiBody.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiGum.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiWhite.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiPants.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiSpikes.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "ShadiiWhite.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiPupil.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiBeard.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiWood.wwmt"));
	grafiki->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "GrafikiCrystal.wwmt"));

	// Text

	dx::SimpleMath::Vector2 posTest;
	posTest.x = 1270;
	posTest.y = 710;
	const wchar_t* inputText = L"Shadii";
	cs::Color4f color(0.034f, 0.255f, 0.0f, 1.0f);
	//These are just test values
	
	// Lights

	shared_ptr<PointLight> point = make_shared<PointLight>();
	point->color = cs::Color3f(0xFFFFFF);
	point->intensity = 5.0f;
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
	// Test empty rotation imgui euler
	Vec3 inputRotation;

	
	
	//Enemy patrolEnemy("Carcinian_Animated.wwm", "Assets/Models/FBX/Rigged/Carcinian_Animations.fbx", Mat::scale3(1.25f, 1.25f, 1.25f) * Mat::translation3(0, -0.6f, 0)* Mat::rotation3(cs::c_pi * -0.5f, 0, 0));
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(1.0f, -5.0f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.5f, -4.2f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.35f, -3.0f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.6f, -1.65f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(3.4f, -0.3f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.35f, 1.0f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.5f, 1.05f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.2f, 0.25f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.25f, -0.8f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(0.9f, -1.5f), true);
	//patrolEnemy.AddCoordinateToPatrolPath(Vec2(2.3f, -1.9f), false);

	Enemy idleEnemy("Carcinian_Animated.wwm", "Assets/Models/FBX/Rigged/Carcinian_Animations.fbx", Mat::scale3(1.25f, 1.25f, 1.25f)* Mat::translation3(0, -0.6f, 0)* Mat::rotation3(cs::c_pi * -0.5f, 0, 0));
	idleEnemy.AddCoordinateToPatrolPath(Vec2(2.0f, 2.0f), true);
	idleEnemy.AddCoordinateToPatrolPath(Vec2(0.0f, 0.0f), true);

	

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
		
		testAnimatorGrafiki.Update(dTime);

		//patrolEnemy.Update(dTime);
		idleEnemy.Update(dTime);

		m_game->Update();
		m_sound->Update();
		rotationY += 0.2f * dTime;
		mesh2->worldMatrix = Mat::translation3(0, -0.8f, 3) * Mat::rotation3(cs::c_pi * -0.5f, -rotationY, 0); // cs::c_pi * 0.9f

		testEmpty.Update(dTime);
		testPlayer.Update(dTime);


		// Draw step
		m_renderer->Draw();

		//#ifdef WW_DEBUG
		m_renderer->BeginGui();
		Move(dTime, &testPlayer);


		if (ImGui::Begin("Animation"))
		{
			ImGui::SliderFloat(testAnimatorGrafiki.loadedAnimations[1].sourceAnimation->name.c_str(), &testAnimatorGrafiki.loadedAnimations[1].influence, 0.0f, 1.0f, "influence = %.3f");
			ImGui::SliderFloat(testAnimatorGrafiki.loadedAnimations[2].sourceAnimation->name.c_str(), &testAnimatorGrafiki.loadedAnimations[2].influence, 0.0f, 1.0f, "influence = %.3f");
			ImGui::SliderFloat("Speed2", &speed2, 0.0f, 3.0f, "speed = %.3f");
			testAnimatorGrafiki.loadedAnimations[0].speed = speed2;
			testAnimatorGrafiki.loadedAnimations[1].speed = speed2;
			testAnimatorGrafiki.loadedAnimations[2].speed = speed2;
		}
		ImGui::End();


		if (ImGui::Begin("EmptyTransform"))
		{
			
			ImGui::Text("Player Parent transform stuff");
			ImGui::DragFloat3("position", (float*)&testEmpty.transform.position, 0.1f);
			ImGui::DragFloat3("rotation", (float*)&inputRotation, 0.1f);
			ImGui::DragFloat3("scale", (float*)&testEmpty.transform.scale, 0.1f);
			testEmpty.transform.rotation = Quaternion::GetEuler(inputRotation);
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
		cs::Vec3 delta = Vec3(mouseState.y, mouseState.x, 0.0f);

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
			Quaternion slerped = Lerp( cameraCurrentRot, conj2, dTime * 5 );
			camera.SetRotation(slerped);
		}
	}
	camera.Update();
}
