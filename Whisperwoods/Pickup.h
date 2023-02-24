#pragma once
#include "GameObject.h"
#include "MeshRenderable.h"
#include "Player.h"


class Pickup : public GameObject
{
public:
	Pickup(const Player* player, Vec2 mapCoordPos);
	virtual ~Pickup();

	virtual void Update(float deltaTime) override;
	virtual void OnPickup(float deltaTime) = 0;

protected:
	float m_pickupRadius;

	const Vec3* m_playerPositionPtr;
	shared_ptr<MeshRenderableStatic> m_renderable;
};
