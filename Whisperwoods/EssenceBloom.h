#pragma once
#include "Pickup.h"
class EssenceBloom sealed : public Pickup
{
public:
	EssenceBloom(Player* player, Vec2 mapCoordPos);
	virtual ~EssenceBloom();

	void OnPickup(float deltatime) override;
	bool IsRemovable() override;

private:
	float* m_staminaRecovery;
	Player* m_player;
	bool m_isAlive;
};

