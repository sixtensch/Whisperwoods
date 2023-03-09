#pragma once

#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "EssenceBloom.h"
#include "StaticObject.h"
#include "Light.h"
#include "SoundResource.h"
#include "LevelHandler.h"
#include "TextRenderable.h"

constexpr float STAMINA_DECAY_MULTIPLIER = 0.27f;
constexpr float MAX_STAMINA_STARTING_VALUE = 10.0f;

class LevelHandler;

class Game sealed
{
	bool m_isCutscene;

	float m_initialCamFov;
	float m_totalFovDelta;
	float m_currentStamina;
	float m_deltaTime;
	float m_closestDistance;
	bool m_isSeen;

	void UpdateGameplayVars( Renderer* renderer );
	
	void UpdateGameObjects();

	void UpdateEnemies( Renderer* renderer );

	void UpdateRoomAndTimeSwappingLogic( Renderer* renderer );

	void DrawIMGUIWindows();

	void CinematicUpdate();

public:
	Game();
	~Game();

	void Update(float deltaTime, Renderer* renderer);

	void Init();
	void DeInit();

	void LoadHubby();
	void LoadTest();
	void LoadGame(uint gameSeed, uint roomCount);
	void UnLoadPrevious();

	Player* GetPlayer();

	void SetCutSceneMode( bool value );

	float GetPowerCooldown();
	float GetMaxPowerCooldown();
	float GetMaxStamina();

	void GodMode(bool godMode);

private:
	void ChangeTimeline(Renderer* renderer);
	void UpdateTimeSwitchBuffers(Renderer* renderer);
	void UpdateEnemyConeBuffers(Renderer* renderer);

	// Time switch functions
	bool IsAllowedToSwitch();
	bool ChargeIsDone();
	bool SwitchIsDone();
	void LoadRoom(Level* level);
	void UnloadRoom();
	bool IsDetected(float deltaTime, float enemyDistance, float maximalDistance);
	void LowerToFloor(float deltaTime);

	void SoundUpdate(float deltaTime);

public:
	//Camera* m_camera;
	std::unique_ptr<LevelHandler>	m_levelHandler;
private:

	Vec3 dirLightOffset;
	shared_ptr<Player> m_player;
	shared_ptr<AudioSource> m_audioSource;
	shared_ptr<DirectionalLight> m_directionalLight;

	float m_musicVol = 0.3;
	shared_ptr<AudioSource> m_musicPresent;
	shared_ptr<AudioSource> m_musicFuture;
	shared_ptr<AudioSource> m_musicDetected;

	float m_hornVol = 1.0f;
	shared_ptr<AudioSource> m_enemyHorn;

	cs::List<shared_ptr<MeshRenderableStatic>> m_testRenderables;
	cs::List<MaterialResource> m_testMaterials;
	int m_testRep;
	int m_testCount;
	uint m_testSeed;

	// Current room data
	cs::List<shared_ptr<PointLight>> m_pointLights;
	cs::List<shared_ptr<SpotLight>> m_spotLights;
	cs::List<shared_ptr<Pickup>> m_pickups;
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

	EnvironmentalizeParameters m_envParams;
	
	bool m_isCutScene;
	bool m_isHubby;
	bool m_isInFuture;
	bool m_isSwitching; // Switching = is true during whole duration (start + end) of time switch.
	bool m_finishedCharging; // Charging = only first part of whole duration (start)
	float m_maxStamina;

	bool m_testTunnel;

	float m_detectionLevelGlobal; //how detected you currently are
	float m_detectionLevelFloor; // minimum detection that detection goes down to
	bool m_reachedLowestStamina;
	float m_camFovChangeSpeed;

	const float m_detectionRate = 0.4f;
	const float m_detCrouchMultiplier = 0.6f;
	const float m_detSprintMultiplier = 1.3f;
	const float m_detDistMultiplier = 1.5f;

	const float m_timeBeforeDetectionLowers = 4.0f; //in seconds
	float m_timeUnseen = 0.0f; // for determining when to derease global detection
	float m_dangerousTimeInFuture = 0.0f;// time in seconds
	const float m_timeYouSurviveInFuture = 2.0f;// time in seconds
	const float m_timeAbilityCooldown = 3.0f; // time in seconds
	float m_coolDownCounter; 

	Vec3 m_fogFocus;
	float m_fogRadius;

	bool m_godMode;
	 
	TimeSwitchValues m_switchVals;
};

