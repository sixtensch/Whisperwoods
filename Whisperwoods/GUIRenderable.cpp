#include "core.h"
#include "GUIRenderable.h"
#include "RenderCore.h"

GUIRenderable::GUIRenderable(uint id, const ModelStaticResource* modelStatic, Mat4 worldMatrix)
{
	this->id = id;
	this->worldMatrix = worldMatrix;
	this->pipelineType = PipelineTypeGUI;
	this->enabled = true;
	m_model = modelStatic;
	//m_elementRef = elementRef;
}

void GUIRenderable::Draw(const DrawInfo& drawInfo) const
{
	uint vertexStride = sizeof(VertexTextured);
	uint vertexOffset = 0;

	uint indexCount = m_model->indexCounts[0];
	uint indexOffset = 0;

	drawInfo.core->SetVertexBuffer(m_model->vertexBuffer, vertexStride, vertexOffset);
	drawInfo.core->SetIndexBuffer(m_model->indexBuffer, indexOffset);

	for (int i = 0; i < m_model->startIndicies.Size(); i++)
	{
		//m_materials.BindIndex(drawInfo.core, i);
		int currentSize = m_model->indexCounts[i];
		int currentStart = (uint)m_model->startIndicies[i];
		drawInfo.core->DrawIndexed(currentSize, currentStart, 0);
	}
}
