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

struct FloorParameters
{
	uint seed;
	uint roomCount;

	uint pushSteps = 3; // Steps to pad rooms to create even spacing
	uint angleSteps = 1; // Steps to move rooms around to 
};

class LevelHandler
{
public:
	LevelHandler();
	LevelHandler(const LevelHandler&) = delete;
	LevelHandler(LevelHandler&&) = delete;

	shared_ptr<uint8_t> GenerateFloorImage(int sizeX, int sizeY, LevelFloor floorRef);

	void LoadFloors();
	void LoadTutorial();
	void GenerateFloor(LevelFloor* outFloor, FloorParameters fParams, EnvironmentalizeParameters eParams);
	void GenerateTestFloor(LevelFloor* outFloor, EnvironmentalizeParameters params);
	void GenerateHubby( LevelFloor* outFloor, EnvironmentalizeParameters params);

private:
	struct RoomPrimer
	{
		cs::List<int> connections;
		Vec2 position;
		Vec2 push;

		int networkIndex = -1;

		int levelIndex = -1;
		cs::List<float> angles;
		cs::List<float> angleDeviations;
		float angleDeviationScore = 0.0f;
		float angleOffset = 0.0f;
		uint connectionOffset = 0;

		bool essenceBloom = false;
	};
	struct TunnelPrimer
	{
		uint start;
		uint end;
	};
	struct TunnelPrimerNetwork
	{
		cs::List<TunnelPrimer> tunnels;
		bool merged = false;
	};
	struct FloorPrimer
	{
		cs::Random r;

		cs::List<TunnelPrimerNetwork> networks;
		cs::List<RoomPrimer> rooms;
	};

	void Environmentalize(Level& l, EnvironmentalizeParameters parameters);
	void AddLevelName(LevelFloor& f, string name);

	int GetLevelByName(string name);

	void CreateNodes(FloorPrimer& f, uint roomCount, uint pushSteps);
	bool TryConnecting(FloorPrimer& f, float dotThreshold);
	bool TryLeveling(FloorPrimer& f, bool repeats, uint roomAttempts);
	bool TryFinalizing(FloorPrimer& f);
	void AngleRoom(FloorPrimer& f, RoomPrimer& r);
	void AngleRooms(FloorPrimer& f);
	void EvaluateRoom(FloorPrimer& f, uint index);
	float EvaluateDeviation(RoomPrimer& r, const LevelResource* level);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;
};
