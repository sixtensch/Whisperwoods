#pragma once

#include "LevelResource.h"
#include "Level.h"

#include <unordered_map>

class LevelHandler
{
public:
	LevelHandler();
	LevelHandler(const LevelHandler&) = delete;
	LevelHandler(LevelHandler&&) = delete;

	void LoadFloors();
	void GenerateFloor(LevelFloor* outFloor);
	void GenerateTestFloor(LevelFloor* outFloor);

private:
	void Environmentalize(Level& l, cs::Random& r);
	void AddLevelName(LevelFloor& f, string name);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;

	// One list per number of entrances
	cs::List<Level> m_levels[3];
};

