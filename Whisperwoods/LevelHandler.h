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
	void GenerateFloor(LevelFloor* outFloor, uint seed, uint roomCount, uint pushSteps);
	void GenerateTestFloor(LevelFloor* outFloor);

private:
	struct RoomPrimer
	{
		cs::List<uint> connections;
		Vec2 position;
		Vec2 push;

		int networkIndex = -1;
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

	void CreateNodes(FloorPrimer& f, uint roomCount, uint pushSteps);
	bool TryConnecting(FloorPrimer& f);
	bool TryLeveling(FloorPrimer& f);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;

	// One list per number of entrances
	cs::List<Level> m_levels[3];
};

