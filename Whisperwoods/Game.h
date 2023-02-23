#pragma once

#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "StaticObject.h"
#include "Light.h"
#include "SoundResource.h"

constexpr float STAMINA_DECAY_MULTIPLIER = 1.2f;

class LevelHandler;

class Game sealed
{
public:
	Game();
	~Game();

	void Update(float deltaTime, Renderer* renderer);

	void Init();
	void DeInit();

	void LoadHubby();
	void LoadTest();
	void LoadGame(uint gameSeed);
	void UnLoadPrevious();

	Player* GetPlayer();

private:
	void ChangeTimeline(Renderer* renderer);
	void UpdateTimeSwitchBuffers(Renderer* renderer);

	// Time switch functions
	bool IsAllowedToSwitch();
	bool ChargeIsDone();
	bool SwitchIsDone();
	void LoadRoom(Level* level);
	void UnloadRoom();
	bool IsDetected(float deltaTime);
	void LowerToFloor(float deltaTime);


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
	shared_ptr<Room> m_currentRoom;

private:

	// TODO: Might benefit of becoming more of a CLASS that HANDLES time switching.
	struct TimeSwitchValues {
		float chargeDuration;
		float falloffDuration;
		float switchCooldown;

		float timeSinceSwitch;
	};


	bool m_isHubby;
	bool m_isInFuture;
	bool m_isSwitching; // Switching = is true during whole duration (start + end) of time switch.
	bool m_finishedCharging; // Charging = only first part of whole duration (start)
	float m_maxStamina;

	float m_detectionLevelGlobal;
	float m_detectionLevelFloor;
	bool m_reachedLowestStamina;
	float m_camFovChangeSpeed;

	const float m_detectionRate = 0.2;
	const float m_timeBeforeDetectionLowers = 10.0f; //in seconds
	float m_timeUnseen = 0.0f; // for determining when to derease global detection
	float m_dangerousTimeInFuture = 0.0f;
	const float m_timeYouSurviveInFuture = 2.5f;


	TimeSwitchValues m_switchVals;
};

