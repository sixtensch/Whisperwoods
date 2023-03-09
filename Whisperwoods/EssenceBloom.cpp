#include "core.h"
#include "EssenceBloom.h"
#include "Renderer.h"
#include "Resources.h"
#include "SoundResource.h"

EssenceBloom::EssenceBloom(Player* player, Vec2 mapCoordPos)
	: Pickup(player, mapCoordPos),
	m_staminaRecovery(0), m_isAlive(true)
	
{
	std::string modelResource = "EssenseBloom.wwm";
	m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_renderable->enabled = true;
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix * Mat::rotation3(cs::c_pi * -0.5f, 0.0f, 0.0f);
	m_renderable->pipelineType = PipelineTypeStandard;
	
	m_player = player;

	m_renderable->Materials().AddMaterial(Resources::Get().GetMaterial("EssenseBloom.wwmt"));
	m_pickupRadius = 0.5f;

	m_soundSource = make_unique<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), m_Vol, 1.5f, 5.0f, 20.0f, (Resources::Get().GetSound("EssenceBloom.mp3"))->currentSound);
	m_soundSource->mix2d3d = 0.0f;
}

EssenceBloom::~EssenceBloom()
{}

void EssenceBloom::OnPickup(float deltatime)
{
	m_renderable->enabled = false;

	// Give player stamina
	m_player->hasPickedUpEssenceBloom = true;
	m_isAlive = false;
	// Play animation

	m_soundSource->Play();
}

bool EssenceBloom::IsRemovable()
{
	return !m_isAlive;
}
