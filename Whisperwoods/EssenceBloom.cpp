#include "core.h"
#include "EssenceBloom.h"
#include "Renderer.h"

EssenceBloom::EssenceBloom(Player* player, Vec2 mapCoordPos)
	: Pickup(player, mapCoordPos),
	m_staminaRecovery(0), m_isAlive(true)
	
{
	std::string modelResource = "ShadiiTest.wwm";
	m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_renderable->enabled = true;
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix;
	m_renderable->pipelineType = PipelineTypeStandard;
	
	m_player = player;
	m_pickupRadius = 0.5f;
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
}

bool EssenceBloom::IsRemovable()
{
	return !m_isAlive;
}
