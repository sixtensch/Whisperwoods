#pragma once
#include "Core.h"

#include <string.h>

#define DEFAULT_RESOURCE_NAME "Default Resource"

class BasicResource {

public:

	BasicResource() {};
	BasicResource(const std::string name) 
		: resourceName(name) {};

public:
	std::string resourceName;
};

