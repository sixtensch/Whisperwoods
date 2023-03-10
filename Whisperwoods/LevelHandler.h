#pragma once

#include "LevelResource.h"
#include "Level.h"

// Includes to make creation of floor texture possible in constructor
#include "GUI.h"

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

	shared_ptr<uint8_t> GenerateFloorImage(LevelFloor* floorRef);
	void SetFloormapFocusRoom(Level* level);

	void LoadFloors();
	void GenerateTutorial(LevelFloor* outFloor, EnvironmentalizeParameters params);
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

	int Get1DPixelPosFromWorld(Vec2 position);
	int GetSafe1DPixelPosFromUV(Vec2 uv);

	Vec2 GetFloorUVFromPos(Vec2 position);
	Vec2 GetRoomFlatenedPos(Vec3 roomPosition);

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

	void Unprime(FloorPrimer& primer, LevelFloor& f, EnvironmentalizeParameters parameters);

private:
	static LevelHandler* s_handler;

	// One list per number of entrances
	cs::List<uint> m_resourceIndices[3];
	cs::List<shared_ptr<LevelResource>> m_resources;


	// Height is assumed to be z dimension.
	float m_minimapWorldMinHeight = FLT_MAX;
	float m_minimapWorldMaxHeight = -FLT_MAX;
	// Width is assumed to be x dimension.
	float m_minimapWorldMinWidth = FLT_MAX;
	float m_minimapWorldMaxWidth = -FLT_MAX;

	shared_ptr<GUI> m_floorMinimapGUI;
	shared_ptr<GUIElement> m_floorMinimapGUIElement;
};
