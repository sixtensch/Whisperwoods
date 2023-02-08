#pragma once

#include "BasicResource.h"

enum ShaderType
{
	ShaderTypeVertex,
	ShaderTypeGeometry,
	ShaderTypeDomain,
	ShaderTypeHull,
	ShaderTypePixel,
	ShaderTypeCompute,

	ShaderTypeCount		// <-- Keep last
};

struct ShaderResource : public BasicResource {

	ShaderResource()
		: ShaderResource("Default shader resource") {}

	ShaderResource(const std::string name)
		: BasicResource(name), shaderVar() {}

	int shaderVar;
};