#include "core.h"
#include "Game.h"
#include "LevelImporter.h"
#include "LevelHandler.h"
#include "SoundResource.h"
#include "Resources.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Update(float deltaTime)
{
	m_player->Update(deltaTime);
	m_currentRoom->Update(deltaTime);

	for (int i = 0; i < m_enemies.Size(); i++)
	{
		m_enemies[i]->Update(deltaTime);
		m_enemies[i]->SeesPlayer(Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z), *m_audioSource, *m_currentRoom);
	}
	
	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update(deltaTime);
	}

	Renderer::SetPlayerMatrix(m_player->transform.worldMatrix);
}

void Game::Init()
{
	// Audio test startup

	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	m_audioSource = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.2f, 1.1f, 0.0f, 10.0f, soundPtr);
	m_audioSource->Play();



	// Level handling

	m_levelHandler = std::make_unique<LevelHandler>();
	m_levelHandler->LoadFloors();



	// In-world objects and entities

	m_player = shared_ptr<Player>(new Player("Shadii_Rigged_Optimized.wwm", "Shadii_Animations.wwa", Mat::translation3(0.0f, 0.0f, 0.0f) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0)));

	m_directionalLight = Renderer::GetDirectionalLight();
	m_directionalLight->transform.position = { 0, 10, 0 };
	m_directionalLight->transform.SetRotationEuler({ -dx::XM_PIDIV4, 0.0f, 0.0f }); // Opposite direction of how the light should be directed
	m_directionalLight->diameter = 22.0f;
	m_directionalLight->intensity = 1.2f;
	m_directionalLight->color = cs::Color3f(0xFFFFD0);
}

void Game::DeInit()
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

void Game::LoadTest()
{
	m_levelHandler->GenerateTestFloor(&m_floor);

	AddRoom(&m_floor.rooms[0]);
	LoadRoom(0);

	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;

	m_enemies.Add(shared_ptr<Enemy>(new Enemy("Carcinian_Animated.wwm", "Carcinian_Animations.wwa", Mat::scale3(1.25f, 1.25f, 1.25f) * Mat::translation3(0, 0, 0) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < m_rooms[0]->m_levelResource->patrolsClosed[i].controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(m_rooms[0]->m_levelResource->patrolsClosed[i].controlPoints[j]);
			Vec3 enemyPos = m_rooms[0]->bitMapToWorldPos(bitPos);
			m_enemies[0]->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), true);
		}
	}
}

void Game::LoadGame(uint gameSeed)
{
	LevelFloor floor = {};
	m_levelHandler->GenerateFloor(&floor);

	for (int i = 0; )

	Level& level = floor.rooms[0];
	Renderer::LoadEnvironment(&level);
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::AddRoom(Level* level)
{
	Mat4 roomMatrix =
		Mat::scale3(level->resource->worldWidth, 1.0f, level->resource->worldHeight) *
		Mat::translation3(0.0f, 0.0f, 0.0f) *
		Mat::rotation3(0.0f, 0.0f, 0.0f);

	m_rooms.Add(shared_ptr<Room>(new Room(level, "room_plane.wwm", roomMatrix)));
	m_rooms.Back()->transform.rotation = Quaternion::GetEuler({0, 0, 0});
}

void Game::ClearRooms()
{
	m_rooms.Clear();
}

void Game::LoadRoom(uint index)
{
	shared_ptr<Room> r = m_rooms[index];

	Renderer::LoadEnvironment(r->m_level);

	m_currentRoom = r.get();
	m_player->currentRoom = r.get();
}

void Game::UnloadRoom(uint index)
{
}
