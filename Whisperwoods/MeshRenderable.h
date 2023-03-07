#pragma once

#include "Renderable.h"
#include "ModelResource.h"
#include "MaterialResource.h"

class MeshRenderableStatic : public WorldRenderable
{
public:
	MeshRenderableStatic(uint id, const ModelStaticResource* model, Mat4 worldMatrix);

	void Draw(const DrawInfo& drawInfo) const override;
	MaterialBundle& Materials();

private:
	MaterialBundle m_materials;
	const ModelStaticResource* m_model;
};


class MeshRenderableRigged : public WorldRenderable
{
public:
	MeshRenderableRigged(uint id, const ModelRiggedResource* model, Mat4 worldMatrix);

	cs::List<DirectX::XMFLOAT4X4> instanceBoneMatricies;

	void Draw(const DrawInfo& drawInfo) const override;
	MaterialBundle& Materials();

private:
	MaterialBundle m_materials;
	const ModelRiggedResource* m_model;
};


class MeshRenderableTerrain : public WorldRenderable
{
public:
	MeshRenderableTerrain( uint id, const ModelStaticResource* model, Mat4 worldMatrix );

	//const TextureResource* m_BitMap;

	void Draw( const DrawInfo& drawInfo ) const override;
	MaterialBundle& Materials();

private:
	MaterialBundle m_materials;
	const ModelStaticResource* m_model;
};