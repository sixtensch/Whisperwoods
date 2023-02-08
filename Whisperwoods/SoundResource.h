#pragma once
#include "BasicResource.h"

#include <fmod.hpp>

struct SoundResource : public BasicResource {

	SoundResource()
		: SoundResource("Default sound resource") {}

	SoundResource(const std::string name)
		: BasicResource(name), currentSound(nullptr) {}

	FMOD::Sound* currentSound;
};