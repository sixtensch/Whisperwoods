#include "core.h"
#include "EssenceBloom.h"
#include "Renderer.h"

EssenceBloom::EssenceBloom() : m_staminaRecovery(0)
{
	std::string modelResource;
	m_renderable = Renderer::CreateMeshStatic(modelResource);

	m_pickupRadius = 4.0f;
}

EssenceBloom::~EssenceBloom()
{}

void EssenceBloom::OnPickup(float deltatime)
{
	m_renderable->enabled = false;
}
