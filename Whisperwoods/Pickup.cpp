#include "core.h"
#include "PickUp.h"

Pickup::Pickup() : m_pickupRadius(0), m_playerPositionPtr(nullptr)
{}
Pickup::~Pickup()
{}


void Pickup::Init(const Player& player)
{
	m_playerPositionPtr = &player.transform.position;
}


void Pickup::Update(float deltaTime)
{
	if ((this->transform.position - *m_playerPositionPtr).Length() <= m_pickupRadius)
		OnPickup(deltaTime);

	// Do animation or whatever
	//...
}
