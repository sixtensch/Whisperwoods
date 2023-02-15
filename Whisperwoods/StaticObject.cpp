#include "Core.h"
#include "StaticObject.h"
#include "Renderer.h"
#include "Resources.h"

StaticObject::StaticObject( std::string modelResource, Mat4 modelOffset, std::vector<std::string> materials )
{
	// Create renderable
	m_modelResource = modelResource;
	m_renderable = Renderer::CreateMeshStatic( m_modelResource );
	m_renderable->worldMatrix = modelOffset; // temp
	Resources& resources = Resources::Get();

	// Add all materials
	for (int i = 0; i < materials.size(); i++)
	{
		m_renderable->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, materials[i]));
	}
}

void StaticObject::Update( float delta_time )
{
	transform.CalculateWorldMatrix(); // TODO: Change to other system later
	m_renderable->worldMatrix = transform.worldMatrix * m_modelOffset;
}
