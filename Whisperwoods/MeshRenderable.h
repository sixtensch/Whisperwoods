#pragma once

#include "Renderable.h"
#include "ModelResource.h"

class MeshRenderableStatic : public WorldRenderable
{
public:
	MeshRenderableStatic(uint id, const ModelStaticResource* model, Mat4 worldMatrix);

	void Draw(const DrawInfo& drawInfo) const override;

private:
	const ModelStaticResource* m_model;
};


class MeshRenderableRigged : public WorldRenderable
{
public:
	MeshRenderableRigged(uint id, const ModelRiggedResource* model, Mat4 worldMatrix);

	void Draw(const DrawInfo& drawInfo) const override;

private:
	const ModelRiggedResource* m_model;
};
