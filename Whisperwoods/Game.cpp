#include "core.h"
#include "Game.h"
#include "LevelImporter.h"
#include "LevelHandler.h"
#include "SoundResource.h"
#include "Resources.h"
#include "Input.h"

Game::Game()
{
	m_future = false;
	m_stamina = 10.0f;
}

Game::~Game()
{
}

void Game::Update(float deltaTime, Renderer* renderer)
{
	m_player->Update(deltaTime);
	m_floors[0]->rooms[0]->Update(deltaTime);
	m_enemies[0]->Update(deltaTime);
	
	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update(deltaTime);
	}

	m_enemies[0]->SeesPlayer(Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z), *(m_floors[0]->rooms[0].get()), *m_audioSource);

	Renderer::SetPlayerMatrix(m_player->transform.worldMatrix);

	if (Input::Get().IsKeyPressed(KeybindPower))
	{
		ChangeTimeline(renderer);
	}
	m_stamina -= deltaTime * STAMINA_DECAY_MULTIPLIER * m_future;

	
	if ( m_stamina < 0.f )
	{
		/// D E A T H ///
		ChangeTimeline(renderer);
	}
}

void Game::InitGame(Renderer* const renderer)
{
	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	m_audioSource = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.2f, 1.1f, 0.0f, 10.0f, soundPtr);
	//m_audioSource->Play();

	m_player = shared_ptr<Player>(new Player("Shadii_Rigged_Optimized.wwm", "Shadii_Animations.wwa", Mat::translation3(0.0f, 0.0f, 0.0f) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0)));

	m_levelHandler = std::make_unique<LevelHandler>();
	m_levelHandler->LoadFloors();

	LevelFloor floor = {};
	m_levelHandler->GenerateFloor(&floor);

	Level& level = floor.rooms[0];
	Renderer::LoadEnvironment(&level);

	// Test code below.
	//LevelResource level = {};
	//LevelImporter::ImportImage("Examplemap.png", renderer->GetRenderCore(), &level);
	Mat4 roomScale = Mat::scale3(level.resource->worldWidth, 1.0f, level.resource->worldHeight);
	Mat4 roomPos = Mat::translation3(0.0f, 0.0f, 0.0f);
	Mat4 roomRot = Mat::rotation3(0.0f, 0.0f, 0.0f);
	Mat4 roomCombined = roomScale * roomPos * roomRot;
	//Room testRoom("room_plane.wwm", "Examplemap.png", roomCombined, renderer);
	//testRoom.transform.rotation = Quaternion::GetEuler({ 0, cs::c_pi * 0.5f ,0 });
	
	m_floors.Add(shared_ptr<Floor>(new Floor));
	m_floors[0]->active = true;
	m_floors[0]->startRoom = floor.startRoom;
	m_floors[0]->startPos = floor.startPosition;
	m_floors[0]->rooms.Add(shared_ptr<Room>(new Room(&level, "room_plane.wwm", roomCombined, renderer)));
	m_floors[0]->rooms[0]->transform.rotation = Quaternion::GetEuler({ 0, 0 /*cs::c_pi * 0.5f*/ ,0 });
	m_player->currentRoom = m_floors[0]->rooms[0].get();

	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;

	// Create the test scene objects
	//m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "Ground.wwm", worldCombined, { "TestSceneGround.wwmt" })));
	//m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "BigTrees.wwm", worldCombined, { "TestSceneBigTree.wwmt" })));
 //   m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "BigPlants.wwm", worldCombined, { "TestSceneBanana.wwmt" })));
 //   m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "SmallPlants.wwm", worldCombined, { "TestSceneTopDownPlant.wwmt" })));
 //   m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "MediumTrees.wwm", worldCombined, { "TestSceneMediumTree.wwmt" } )));
 //   m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "Stones.wwm", worldCombined, { "TestSceneStones.wwmt" })));
 //   m_staticObjects.Add(shared_ptr<StaticObject>(new StaticObject( "Grafitree.wwm", worldCombined, { "TestSceneGrafitree.wwmt" })));

	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_floors[0]->rooms[0]->AddChild(m_staticObjects[i].get());
	}

	// Create the directional light
	m_directionalLight = Renderer::GetDirectionalLight();
	m_directionalLight->transform.position = { 0, 10, 0 };
	m_directionalLight->transform.SetRotationEuler({ -dx::XM_PIDIV4, 0.0f, 0.0f }); // Opposite direction of how the light should be directed
	m_directionalLight->diameter = 22.0f;
	m_directionalLight->intensity = 1.2f;
	m_directionalLight->color = cs::Color3f(0xFFFFD0);

	//m_enemies.Add(shared_ptr<Enemy>(new Enemy("Carcinian_Animated.wwm", "Carcinian_Animations.wwa",
	//	Mat::scale3(1.25f, 1.25f, 1.25f) * Mat::translation3(0, -0.6f, 0) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(1.0f, -5.0f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(2.5f, -4.2f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(3.35f, -3.0f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(3.6f, -1.65f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(3.4f, -0.3f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(2.35f, 1.0f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(0.5f, 1.05f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(0.2f, 0.25f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(0.25f, -0.8f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(0.9f, -1.5f), true);
	//m_enemies[0]->AddCoordinateToPatrolPath(Vec2(2.3f, -1.9f), false);

	m_enemies.Add(shared_ptr<Enemy>(new Enemy("Carcinian_Animated.wwm", "Carcinian_Animations.wwa", Mat::scale3(1.25f, 1.25f, 1.25f) * Mat::translation3(0, 0, 0) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));
	for (int i = 0; i < 1/*testRoom.m_levelResource.patrolsClosed.Size()*/; i++)
	{
		for (int j = 0; j < m_floors[0]->rooms[0]->m_levelResource->patrolsClosed[i].controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(m_floors[0]->rooms[0]->m_levelResource->patrolsClosed[i].controlPoints[j]);
			Vec3 enemyPos = m_floors[0]->rooms[0]->bitMapToWorldPos(bitPos);
			m_enemies[0]->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), true);
			Point2 test = m_floors[0]->rooms[0]->worldToBitmapPoint(enemyPos);
			enemyPos = Vec3(0, 0, 0);
		}
	}
}

void Game::DeInitGame()
{
	// Audio test 2 shutdown
	m_audioSource->pitch = 0.75f;
	m_audioSource->Play();

	int indexer = 0;
	while (m_audioSource->IsPlaying())
	{
		Sound::Get().Update();
		indexer++;
	}
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::ChangeTimeline(Renderer* renderer)
{
	m_future = !m_future * (m_stamina > 0.f);
	renderer->SetTimelineState(m_future);
}
