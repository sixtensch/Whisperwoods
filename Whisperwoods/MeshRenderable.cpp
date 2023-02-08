#include "core.h"
#include "MeshRenderable.h"
#include "RenderCore.h"

MeshRenderableStatic::MeshRenderableStatic(uint id, const ModelStaticResource* model, Mat4 worldMatrix)
{
	this->id = id;
	this->worldMatrix = worldMatrix;
	this->pipelineType = PipelineTypeStandard;
	this->enabled = true;

	m_model = model;
}

void MeshRenderableStatic::Draw(const DrawInfo& drawInfo) const
{
	uint vertexStride = sizeof(VertexTextured);
	uint vertexOffset = 0;

	uint indexCount = m_model->indicies.Size();
	uint indexOffset = 0;

	drawInfo.core->SetVertexBuffer(m_model->vertexBuffer, vertexStride, vertexOffset) ; 
	drawInfo.core->SetIndexBuffer(m_model->indexBuffer, indexOffset) ;

	drawInfo.core->DrawIndexed(indexCount, 0, 0);
}
