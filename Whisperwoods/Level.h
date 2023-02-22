#pragma once

#include "LevelResource.h"

enum LevelAsset : uint
{
	LevelAssetBush1,	// BananaPlant
	LevelAssetBush2,	// BananaPlant

	LevelAssetCount
};

struct Level;
struct LevelTunnel;

struct Level
{
	Vec3 position;
	Quaternion rotation;
	float scale;

	// In world space
	cs::List<Mat4> instances[LevelAssetCount];

	// Indices into LevelFloor::tunnels
	cs::List<uint> connections;

	const LevelResource* resource;
};

struct LevelTunnel
{
	// Indices into LevelFloor::rooms
	uint startRoom;
	uint endRoom;

	// Same as source/destination exits
	LevelExit startExit;
	LevelExit endExit;

	// In world space
	cs::List<Mat4> instances[LevelAssetCount];
};

struct LevelFloor
{
	// In world space
	Vec3 startPosition;

	uint startRoom;

	cs::List<Level> rooms;
	cs::List<LevelTunnel> tunnels;
};
