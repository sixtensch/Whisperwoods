#pragma once

#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "StaticObject.h"
#include "Light.h"
#include "SoundResource.h"

class LevelHandler;

class Game sealed
{
public:
	Game();
	~Game();

	void Update(float deltaTime);

	void Init();
	void DeInit();

	void LoadTest();
	void LoadGame(uint gameSeed);

	Player* GetPlayer();

private:
	void AddRoom(Level* level);
	void ClearRooms();

	void LoadRoom(uint index);
	void UnloadRoom(uint index);

private:
	std::unique_ptr<LevelHandler>	m_levelHandler;

	shared_ptr<Player> m_player;
	shared_ptr<AudioSource> m_audioSource;
	shared_ptr<DirectionalLight> m_directionalLight;

	// Current room data
	cs::List<shared_ptr<PointLight>> m_pointLights;
	cs::List<shared_ptr<SpotLight>> m_spotLights;
	cs::List<shared_ptr<Enemy>> m_enemies;
	cs::List<shared_ptr<StaticObject>> m_staticObjects;

	// Current floor data
	LevelFloor m_floor;
	Room* m_currentRoom;
	cs::List<shared_ptr<Room>> m_rooms;

};

