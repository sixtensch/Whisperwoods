#include "core.h"
#include "EssenceBloom.h"
#include "Renderer.h"
#include "Resources.h"

EssenceBloom::EssenceBloom(const Player* player, Vec2 mapCoordPos) 
	: Pickup(player, mapCoordPos),
	m_staminaRecovery(0)
{
	std::string modelResource = "EssenseBloom.wwm";
	m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_renderable->enabled = true;
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix * Mat::rotation3(cs::c_pi * -0.5f, 0.0f, 0.0f);
	m_renderable->pipelineType = PipelineTypeStandard;

	m_renderable->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "EssenseBloom.wwmt"));
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
