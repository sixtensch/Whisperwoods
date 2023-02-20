#pragma once

#include "Renderer.h"
#include "Player.h"
#include "Floor.h"
#include "Enemy.h"
#include "StaticObject.h"
#include "Light.h"
#include "LevelHandler.h"
#include "SoundResource.h"

constexpr float STAMINA_DECAY_MULTIPLIER = 1.2f;
class Game sealed
{
public:
	Game();
	~Game();

	void Update(float deltaTime, Renderer* renderer);

	void InitGame(Renderer* const renderer); // TODO: remove the need for renderer when generation is further progressed.
	void DeInitGame();

	Player* GetPlayer();

private:
	void ChangeTimeline(Renderer* renderer);
	void UpdateTimeSwitchBuffers(Renderer* renderer);

	// Time switch functions
	bool IsAllowedToSwitch();
	bool ChargeIsDone();
	bool SwitchIsDone();

	std::unique_ptr<LevelHandler>	m_levelHandler;

	shared_ptr<AudioSource> m_audioSource;

	shared_ptr<Player> m_player;
	shared_ptr<DirectionalLight> m_directionalLight;

	cs::List<shared_ptr<Floor>> m_floors;
	cs::List<shared_ptr<PointLight>> m_pointLights;
	cs::List<shared_ptr<SpotLight>> m_spotLights;
	cs::List<shared_ptr<Enemy>> m_enemies;
	cs::List<shared_ptr<StaticObject>> m_staticObjects;

private:

	// TODO: Might benefit of becoming more of a CLASS that HANDLES time switching.
	struct TimeSwitchValues {
		float chargeDuration;
		float falloffDuration;
		float switchCooldown;

		float timeSinceSwitch;
	};

	bool m_isInFuture;
	bool m_isSwitching; // Switching = state during whole duration of time switch
	bool m_finishedCharging; // Charging = state during duration of charging eon key
	float m_stamina;

	TimeSwitchValues m_switchVals;
};

