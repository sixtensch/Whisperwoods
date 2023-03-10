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
#include "GUI.h"
#include "CutsceneController.h"
#include "Grafiki.h"

constexpr float STAMINA_DECAY_MULTIPLIER = 0.2f;
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

	void UpdateRoomAndTimeSwappingLogic( Renderer* renderer);

	void DrawIMGUIWindows();

	void CinematicUpdate();

	

public:
	Game();
	~Game();

	void Update(float deltaTime, Renderer* renderer);

	void Init();
	void DeInit();

	void InitCutscene();

	

	void LoadHubby();
	void LoadTest();
	void LoadTutorial();
	void LoadGame(uint gameSeed, uint roomCount);
	void UnLoadPrevious();

	Player* GetPlayer();
	void MovePlayer(Vec3 position, Vec3 direction);

	void Move(float dTime, Player* player, CutsceneController* cutSceneController);
		
	float GetPowerCooldown();
	float GetMaxPowerCooldown();
	float GetMaxStamina();

	void SetGUI(GUI* gui);

	void GodMode(bool godMode);

	bool IsInHubby();

private:
	// Applies current timeline state to all relevant units.
	void ApplyTimelineState(Renderer* renderer);

	void SwapTimeline(Renderer* renderer);
	void ChangeToFutureTimeline(Renderer* renderer);
	void ChangeToPresentTimeline(Renderer* renderer);
	
	void UpdateTimeSwitchBuffers(Renderer* renderer);
	void UpdateEnemyConeBuffers(Renderer* renderer);

	void SoundUpdate(float deltaTime);

	// Time switch functions
	bool IsAllowedToSwitch();
	bool ChargeIsDone();
	bool SwitchIsDone();
	void LoadRoom(uint levelIndex);
	void UnloadRoom();
	bool IsDetected(float deltaTime, float enemyDistance, float maximalDistance);
	void LowerToFloor(float deltaTime);

	void ResetGameplayValues();
	void EndRun(Renderer* renderer);
	void EndRunDueToEnemy(Renderer* renderer);
	void EndRunDueToPoison(Renderer* renderer);
	

public:
	//Camera* m_camera;
	std::unique_ptr<LevelHandler>	m_levelHandler;
	int activeTutorialLevel = 1;
	bool tutorial = false;
	bool showTextForPickupBloom = false;
	bool youWin = false;
	shared_ptr<Grafiki> m_grafiki;

private:

	Vec3 noiseVal1;
	Vec3 noiseVal2;

	Vec3 dirLightOffset;
	shared_ptr<Player> m_player;
	shared_ptr<AudioSource> m_audioSource;
	shared_ptr<DirectionalLight> m_directionalLight;
	cs::Color3f m_ambientColor;
	float m_ambientIntensity;
	cs::Color3f m_futureAmbientColor;
	float m_futureAmbientIntensity;
	cs::Color3f m_directionalColor;
	float m_directionalIntensity;
	cs::Color3f m_futureDirectionalColor;
	float m_futureDirectionalIntensity;

	Vec2 m_vignetteStrengthAndRadius;
	Vec2 m_contrastStrengthAndMidpoint;
	float m_finalBrightness;
	float m_finalSaturation;

	bool firstSet = true;

	float m_musicVol = 0.3f;

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

	shared_ptr<GUI> m_loadScreen;


	// TODO: Might benefit of becoming more of a CLASS that HANDLES time switching.
	struct TimeSwitchValues {
		float chargeDuration;
		float falloffDuration;
		float switchCooldown;

		float timeSinceSwitch;
	};

	EnvironmentalizeParameters m_envParams;
private:

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

	const float m_detectionRate = 0.35f;
	const float m_detCrouchMultiplier = 0.6f;
	const float m_detSprintMultiplier = 1.3f;
	const float m_detDistMultiplier = 1.5f;

	const float m_timeBeforeDetectionLowers = 4.0f; //in seconds
	float m_timeUnseen = 0.0f; // for determining when to derease global detection
	float m_dangerousTimeInFuture = 0.0f;// time in seconds
	const float m_timeYouSurviveInFuture = 2.0f;// time in seconds
	const float m_timeAbilityCooldown = 3.0f; // time in seconds
	float m_coolDownCounter; 

	const float m_timeSwitchTargetFov = cs::c_pi * 0.9f;

	Vec3 m_fogFocus;
	float m_fogRadius;

	bool m_godMode;

	bool m_loadingHubby = false;
	bool m_loadingTutorial = false;
	bool m_loadingGame = false;
	bool m_loadNewFloor = false;
	bool m_deathPoison = false;
	bool m_deathEnemy = false; 
	
	bool m_skipTutorialQuestion = false;

	bool m_cameraPlayer;
	bool m_cameraLock;
	GUI* m_gui;
	shared_ptr<CutsceneController> m_cutsceneController;
	shared_ptr<Cutscene> m_testCutScene;

	void TransitionStart(
		Vec3 exitPosition,
		Vec3 exitDirection,
		uint targetRoom, 
		Vec3 targetPosition,
		Vec3 targetDirection);
	void TransitionLoad();
	void TransitionEnd();
	void TransitionUpdate(float deltaTime);
	void ExecuteLoad(uint targetRoom, Vec3 position, Vec3 direction);

	float m_winTimer = 0.0f;
	float m_timePerEndSlideShow = 7.0f;
	 
	TimeSwitchValues m_switchVals;

	Vec3 m_targetFogFocus;
	float m_targetFogRadius;
	uint m_targetRoom;
	Vec3 m_targetSpawnPosition;
	Vec3 m_targetSpawnDirection;
	Vec3 m_targetCameraPosition;
	Quaternion m_targetCameraDirection;
	float m_transitionTime;
	float m_transitionTimeTarget;
	enum TransitionTarget
	{
		TransitionTargetNone,
		TransitionTargetFree,
		TransitionTargetLoadRoom,
		TransitionTargetExit,
	} m_transitionTarget;
};

