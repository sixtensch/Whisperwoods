#pragma once
#include "Pickup.h"
class EssenceBloom sealed : public Pickup
{
public:
	EssenceBloom();
	virtual ~EssenceBloom();

	void OnPickup(float deltatime) override;

private:
	float m_staminaRecovery;
};

