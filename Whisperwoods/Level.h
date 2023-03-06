#pragma once

#include "LevelResource.h"

enum LevelAsset : uint
{
	LevelAssetBush1,	// BananaPlant
	LevelAssetBush2,	// BananaPlant
	LevelAssetTree1,	// MediumBrowntree
	LevelAssetTree2,	// MediumPinkThistleTree
	LevelAssetTree3,	// GrayFeatherTree
	LevelAssetBigTrunk1,	//ThickBackgroundTrunk
	LevelAssetBigTrunk2,	//SlenderBackgroundTrunk
	LevelAssetStone1,	// SingleStone
	LevelAssetStone2,	// StoneCluster

	LevelAssetCount
};

struct Level;
struct LevelTunnel;
struct LevelTunnelRef;

struct Level
{
	Vec3 position;
	Quaternion rotation;
	float scale;

	bool essenceBloom = false;

	// In world space
	cs::List<Mat4> instances[LevelAssetCount];

	cs::List<LevelTunnelRef> connections;

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

struct LevelTunnelRef
{
	// Indices into LevelFloor::rooms;
	// -1: Entrance to floor
	// -2: Exit from floor
	int targetRoom;

	// Indices into LevelFloor::tunnels
	uint tunnel;
};
