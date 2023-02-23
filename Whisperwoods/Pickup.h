#pragma once
#include "GameObject.h"
#include "MeshRenderable.h"
#include "Player.h"


class Pickup : GameObject
{
public:
	Pickup();
	virtual ~Pickup();

	void Init(const Player& player);
	virtual void Update(float deltaTime) override;
	virtual void OnPickup(float deltaTime) = 0;

protected:
	float m_pickupRadius;

	const Vec3* m_playerPositionPtr;
	shared_ptr<MeshRenderableStatic> m_renderable;
};
