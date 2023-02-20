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

	LoadRoom(&m_floor.rooms[0]);

	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;
}

void Game::LoadGame(uint gameSeed)
{
	m_levelHandler->GenerateTestFloor(&m_floor);

	LoadRoom(&m_floor.rooms[m_floor.startRoom]);

	m_player->transform.position = m_floor.startPosition;

	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::LoadRoom(Level* level)
{
	Mat4 roomMatrix =
		Mat::scale3(level->resource->worldWidth, 1.0f, level->resource->worldHeight) *
		Mat::translation3(level->position.x, level->position.x, level->position.x)*
		level->rotation.Matrix();

	m_currentRoom = shared_ptr<Room>(new Room(level, "room_plane.wwm", roomMatrix));
	m_currentRoom->transform.rotation = Quaternion::GetEuler({ 0, 0, 0 });

	Renderer::LoadEnvironment(m_currentRoom->m_level);

	m_player->currentRoom = m_currentRoom.get();

	for (LevelPatrol& p : level->resource->patrolsClosed)
	{
		m_enemies.Add(shared_ptr<Enemy>(new Enemy(
			"Carcinian_Animated.wwm", 
			"Carcinian_Animations.wwa", 
			Mat::scale3(1.25f, 1.25f, 1.25f) * 
			Mat::translation3(0, 0, 0) * 
			Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));
		
		for (int j = 0; j < p.controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(p.controlPoints[j]);
			Vec3 enemyPos = m_currentRoom->bitMapToWorldPos(bitPos);
			m_enemies.Back()->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), true);
		}
	}

	for (LevelPatrol& p : level->resource->patrolsOpen)
	{
		m_enemies.Add(shared_ptr<Enemy>(new Enemy(
			"Carcinian_Animated.wwm",
			"Carcinian_Animations.wwa",
			Mat::scale3(1.25f, 1.25f, 1.25f) *
			Mat::translation3(0, 0, 0) *
			Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));

		for (int j = 0; j < p.controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(p.controlPoints[j]);
			Vec3 enemyPos = m_currentRoom->bitMapToWorldPos(bitPos);
			m_enemies.Back()->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), false);
		}
	}
}

void Game::UnloadRoom()
{
}

void Game::ChangeTimeline(Renderer* renderer)
{
	m_future = !m_future * (m_stamina > 0.f);
	renderer->SetTimelineState(m_future);
}
