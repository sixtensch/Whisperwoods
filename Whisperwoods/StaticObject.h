#pragma once
#include "GameObject.h"
#include "MeshRenderable.h"

class StaticObject : public GameObject
{
	std::string m_modelResource;
	Mat4 m_modelOffset;
	shared_ptr<MeshRenderableStatic> m_renderable;
public:
	StaticObject( std::string modelResource, Mat4 modelOffset, std::vector<std::string> materials );
	void Update( float delta_time ) override;
};