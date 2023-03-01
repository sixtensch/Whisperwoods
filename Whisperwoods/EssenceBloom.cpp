#include "core.h"
#include "EssenceBloom.h"
#include "Renderer.h"

EssenceBloom::EssenceBloom(const Player* player, Vec2 mapCoordPos) 
	: Pickup(player, mapCoordPos),
	m_staminaRecovery(0)
	
{
	std::string modelResource = "EssenseBloom.wwm";
	m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_renderable->enabled = true;
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix;
	m_renderable->pipelineType = PipelineTypeStandard;

	m_pickupRadius = 0.5f;
}

EssenceBloom::~EssenceBloom()
{}

void EssenceBloom::OnPickup(float deltatime)
{
	m_renderable->enabled = false;
	
	// Give player stamina


	// Play animation
}
