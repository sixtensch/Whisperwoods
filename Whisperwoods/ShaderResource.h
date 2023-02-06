#pragma once

#include "BasicResource.h"

struct ShaderResource : public BasicResource {

	ShaderResource()
		: ShaderResource("Default shader resource") {}

	ShaderResource(const std::string name)
		: BasicResource(name), shaderVar() {}

	int shaderVar;
};