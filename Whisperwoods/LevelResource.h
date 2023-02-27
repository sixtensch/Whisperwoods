#pragma once

#include "BasicResource.h"
#include "TextureResource.h"

enum LevelPixelFlag : byte
{
	LevelPixelFlagImpassable	= 0b001,
	LevelPixelFlagInner		= 0b010,
	LevelPixelFlagCrouchable	= 0b100,

	LevelPixelFlagPassable				= 0,
	LevelPixelFlagPassableCrouchable	= 0 | LevelPixelFlagCrouchable,

	LevelPixelFlagTerrainInner	= LevelPixelFlagImpassable | LevelPixelFlagInner,
	LevelPixelFlagTerrainOuter	= LevelPixelFlagImpassable & ~LevelPixelFlagInner
};

struct LevelPixel
{
	LevelPixelFlag flags;
	float density;
};

struct LevelPatrol
{
	uint enemyIndex;
	cs::List<Vec2> controlPoints;
};

struct LevelExit
{
	Vec2 position;
	Vec2 direction;
	float width;
};

struct LevelPickup
{
	Vec2 position;
};

struct LevelResource : public BasicResource
{
	uint pixelWidth;
	uint pixelHeight;
	float worldWidth;
	float worldHeight;
	
	cs::List<LevelPickup> pickups;

	cs::List<LevelPatrol> patrolsOpen;
	cs::List<LevelPatrol> patrolsClosed;

	cs::List<LevelExit> exits;

	shared_ptr<LevelPixel[]> bitmap;

	const TextureResource* source;
};

