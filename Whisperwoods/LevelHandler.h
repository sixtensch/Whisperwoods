#pragma once

#include "LevelResource.h"
#include "Level.h"

#include <unordered_map>

struct LevelParams
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

	void LoadFloors();
	void GenerateFloor(LevelFloor* outFloor, uint seed, uint roomCount, uint pushSteps);
	void GenerateTestFloor(LevelFloor* outFloor);

private:
	struct RoomPrimer
	{
		cs::List<uint> connections;
		Vec2 position;
		Vec2 push;

		int networkIndex = -1;

		int levelIndex = -1;
		cs::List<float> angles;
		cs::List<float> angleDeviations;
		float angleDeviationScore = 0.0f;
		float angleOffset = 0.0f;
		uint connectionOffset = 0;
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

	void Environmentalize(Level& l, cs::Random& r);
	void AddLevelName(LevelFloor& f, string name);

	int GetLevelByName(string name);

	void CreateNodes(FloorPrimer& f, uint roomCount, uint pushSteps);
	bool TryConnecting(FloorPrimer& f, float dotThreshold);
	bool TryLeveling(FloorPrimer& f, bool repeats, uint roomAttempts);
	void AngleRooms(FloorPrimer& f);
	void EvaluateRoom(FloorPrimer& f, uint index);
	float EvaluateDeviation(RoomPrimer& r, const LevelResource* level);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;
};

