#pragma once

#include "Renderer.h"
#include "Player.h";
#include "Floor.h"
#include "Enemy.h"
#include "StaticObject.h"
#include "Light.h"

class Game sealed
{
public:
	Game();
	~Game();

	void Update(float deltaTime);

	void InitGame(Renderer* const renderer); // TODO: remove the need for renderer when generation is further progressed.

	Player* GetPlayer();

private:

	shared_ptr<Player> m_player;
	shared_ptr<DirectionalLight> m_directionalLight;

	cs::List<shared_ptr<Floor>> m_floors;
	cs::List<shared_ptr<PointLight>> m_pointLights;
	cs::List<shared_ptr<SpotLight>> m_spotLights;
	cs::List<shared_ptr<Enemy>> m_enemies;
	cs::List<shared_ptr<StaticObject>> m_staticObjects;

};

