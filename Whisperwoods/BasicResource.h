#pragma once
#include "Core.h"

#include <string.h>

#define DEFAULT_RESOURCE_NAME "Default Resource"

/*
	Note:
	Inherited resources should have a constructor available for inputting its name.

	Newly defined resources that derive from this class has to define their own allocation
	inside Resources::AllocateResource() with make_shared<NewResourceType>() to be handled properly. 
		
*/

struct BasicResource {

	BasicResource() 
		: BasicResource(DEFAULT_RESOURCE_NAME) {};

	BasicResource(const std::string name) 
		: name(name) {};

	// Name is not for any functional reason. Only for identification in debugging.
	std::string name;
};

