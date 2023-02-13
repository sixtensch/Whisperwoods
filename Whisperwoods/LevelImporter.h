#pragma once

#include "LevelResource.h"

class RenderCore;

class LevelImporter
{
public:
	static bool ImportImage(string textureName, const RenderCore* core, LevelResource* const outLevel);
};

