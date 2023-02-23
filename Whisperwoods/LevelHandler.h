#pragma once

#include "LevelResource.h"
#include "Level.h"

#include <unordered_map>

struct EnvironmentalizeParameters
{
	int spawnSeed;
	int scaleSeed;
	int rotationSeed;
	int diversitySeed;
	float rotateMult;
	float xMult;
	float yMult;
	float scaleBase;
	float scaleMult;
	float densityUnwalkableOuter;
	float densityUnwalkableInner;
	float densityWalkable;
	float minDensity;
	float scaleMultiplierStones;
	float scaleMultiplierTrees;
	float scaleMultiplierFoliage;
	float scaleEffectDensity;
	int edgeSampleDistanceTrunks;
	int edgeSampleDistanceTrees;
	int edgeSampleDistanceStones;
};

class LevelHandler
{
public:
	LevelHandler();
	LevelHandler(const LevelHandler&) = delete;
	LevelHandler(LevelHandler&&) = delete;

	void LoadFloors();
	void GenerateHubby( LevelFloor* outFloor, EnvironmentalizeParameters params);
	void GenerateFloor(LevelFloor* outFloor, EnvironmentalizeParameters params);
	void GenerateTestFloor(LevelFloor* outFloor, EnvironmentalizeParameters params);

private:
	void Environmentalize(Level& l, EnvironmentalizeParameters parameters);
	void AddLevelName(LevelFloor& f, string name);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;

	// One list per number of entrances
	cs::List<Level> m_levels[3];
};

