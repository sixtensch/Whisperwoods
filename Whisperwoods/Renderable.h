#pragma once

#include "Pipeline.h"
#include "DrawInfo.h"

class Renderable
{
public:
	uint id;
	bool enabled;
	PipelineType pipelineType;



	virtual void Draw(const DrawInfo& drawInfo) const = 0;
};

class WorldRenderable : public Renderable
{
public:
	Mat4 worldMatrix;
};