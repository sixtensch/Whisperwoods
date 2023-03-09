#pragma once
#include "Pickup.h"
#include "SoundResource.h"

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

	float m_Vol = 0.5f;
	unique_ptr<AudioSource> m_soundSource;

	float m_ambianceVol = 0.5f;
	unique_ptr<AudioSource> m_ambianceSource;
};

