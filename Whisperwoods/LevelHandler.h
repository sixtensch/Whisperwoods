#pragma once

#include "LevelResource.h"
#include "Level.h"

class LevelHandler
{
public:
	LevelHandler();
	LevelHandler(const LevelHandler&) = delete;
	LevelHandler(LevelHandler&&) = delete;

	void LoadFloors();
	void GenerateFloor(LevelFloor* outFloor);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<LevelResource> m_resources;

	// One list per number of entrances
	cs::List<Level> m_levels[3];
};

