#include "core.h"
#include "PickUp.h"

Pickup::Pickup(const Player* player, Vec2 mapCoordPos) : m_pickupRadius(0), m_playerPositionPtr(nullptr)
{
	m_playerPositionPtr = &player->transform.position;

	transform.position = { mapCoordPos.x, 0, mapCoordPos.y };
}
Pickup::~Pickup()
{}


void Pickup::Update(float deltaTime)
{
	if ((this->transform.position - *m_playerPositionPtr).Length() <= m_pickupRadius)
		OnPickup(deltaTime);

	// Do animation or whatever
	//...
}
