#pragma once
#include "BasicResource.h"

struct SoundResource : public BasicResource {

	SoundResource()
		: SoundResource("Default sound resource") {}

	SoundResource(const std::string name)
		: BasicResource(name), soundVar() {}

	int soundVar;
};