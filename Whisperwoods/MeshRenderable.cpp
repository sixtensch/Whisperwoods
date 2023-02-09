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

	//uint indexCount = m_model->indexCounts[0];
	uint indexOffset = 0;

	drawInfo.core->SetVertexBuffer(m_model->vertexBuffer, vertexStride, vertexOffset) ; 
	drawInfo.core->SetIndexBuffer(m_model->indexBuffer, indexOffset) ;

	for (int i = 0; i < m_model->startIndicies.Size(); i++)
	{
		int currentSize = m_model->indexCounts[i];
		int currentStart = (uint)m_model->startIndicies[i];
		drawInfo.core->DrawIndexed(currentSize, currentStart, 0);
	}
}

MeshRenderableRigged::MeshRenderableRigged(uint id, const ModelRiggedResource* model, Mat4 worldMatrix)
{
	this->id = id;
	this->worldMatrix = worldMatrix;
	this->pipelineType = PipelineTypeStandardRigged;
	this->enabled = true;
	m_model = model;
}

void MeshRenderableRigged::Draw(const DrawInfo& drawInfo) const
{
	uint vertexStride = sizeof(VertexRigged);
	uint vertexOffset = 0;

	//uint indexCount = m_model->indexCounts[0];
	uint indexOffset = 0;

	drawInfo.core->SetVertexBuffer(m_model->vertexBuffer, vertexStride, vertexOffset);
	drawInfo.core->SetIndexBuffer(m_model->indexBuffer, indexOffset);

	for (int i = 0; i < m_model->startIndicies.Size(); i++)
	{
		int currentSize = m_model->indexCounts[i];
		int currentStart = (uint)m_model->startIndicies[i];
		drawInfo.core->DrawIndexed(currentSize, currentStart, 0);
	}
}
