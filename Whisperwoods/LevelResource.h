#pragma once

#include "BasicResource.h"
#include "TextureResource.h"

enum LevelPixel : byte
{
	Impassable	= 0x1,
	Inner		= 0x2,

	Passable		= 0,
	TerrainInner	= Impassable | Inner,
	TerrainOuter	= Impassable & ~Inner
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

struct LevelResource : public BasicResource
{
	uint pixelWidth;
	uint pixelHeight;
	float worldWidth;
	float worldHeight;
	
	cs::List<LevelPatrol> patrolsOpen;
	cs::List<LevelPatrol> patrolsClosed;

	cs::List<LevelExit> exits;

	shared_ptr<LevelPixel[]> bitmap;

	const TextureResource* source;
};

