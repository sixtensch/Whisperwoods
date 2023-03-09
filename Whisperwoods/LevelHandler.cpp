#include "core.h"
#include "LevelHandler.h"
#include "LevelImporter.h"
#include "Renderer.h"

#include <filesystem>

#define LEVEL_MAP_PIXEL_WIDTH 100u
#define LEVEL_MAP_PIXEL_HEIGHT 100u
#define PIXEL_PADDING 10u // TODO: Define and use this better.
#define BYTES_PER_TEXEL 4u

bool Intersect(Vec2 p1a, Vec2 p1b, Vec2 p2a, Vec2 p2b)
{
	// Line AB represented as a1x + b1y = c1
	float a1 = p1b.y - p1a.y;
	float b1 = p1a.x - p1b.x;
	float c1 = a1 * (p1a.x) + b1 * (p1a.y);

	// Line CD represented as a2x + b2y = c2
	float a2 = p2b.y - p2a.y;
	float b2 = p2a.x - p2b.x;
	float c2 = a2 * (p2a.x) + b2 * (p2a.y);

	float determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		return false;
	}
	else
	{
		float x = (b2 * c1 - b1 * c2) / determinant;
		float y = (a1 * c2 - a2 * c1) / determinant;

		return 
			cs::fmin(p1a.x, p1b.x) < x - 0.01f &&
			cs::fmin(p1a.y, p1b.y) < y - 0.01f &&
			cs::fmax(p1a.x, p1b.x) > x + 0.01f &&
			cs::fmax(p1a.y, p1b.y) > y + 0.01f;
	}
}

LevelHandler* LevelHandler::s_handler = nullptr;

LevelHandler::LevelHandler()
{
}

int LevelHandler::Get1DChannelPos(const float minWidth, const float maxWidth, const float minHeight, const float maxHeight, Vec2 position)
{
	float xUVPos = (position.x - minWidth) / (maxWidth - minWidth);
	float yUVPos = (position.y - minHeight) / (maxHeight - minHeight);

	// Simple UV mapping of coordinates.
	return (cs::floor(yUVPos * LEVEL_MAP_PIXEL_HEIGHT) * LEVEL_MAP_PIXEL_WIDTH + cs::floor(xUVPos * LEVEL_MAP_PIXEL_WIDTH)) * BYTES_PER_TEXEL;
}

shared_ptr<uint8_t> LevelHandler::GenerateFloorImage(LevelFloor* floorRef)
{
	// Height is assumed to be z dimension.
	float minMaxHeight[2] = { FLT_MAX, -FLT_MAX };
	// Width is assumed to be x dimension.
	float minMaxWidth[2] = { FLT_MAX, -FLT_MAX };

	cs::List<Vec2> roomPositions = {};
	for (int i = 0; i < floorRef->rooms.Size(); i++)
	{
		Vec3 roomPos = floorRef->rooms[i].position;
		Vec2 roomFlatPos = Vec2(roomPos.x, roomPos.z);

		minMaxHeight[0] = cs::fmin(minMaxHeight[0], roomFlatPos.y);
		minMaxHeight[1] = cs::fmax(minMaxHeight[1], roomFlatPos.y);

		minMaxWidth[0] = cs::fmin(minMaxWidth[0], roomFlatPos.x);
		minMaxWidth[1] = cs::fmax(minMaxWidth[1], roomFlatPos.x);

		roomPositions.Add(roomFlatPos);
		LOG_TRACE("%d - Room pos: %.2f %.2f %.2f", i, roomPos.x, roomPos.y, roomPos.z);
	}

	shared_ptr<uint8_t> returnData = shared_ptr<uint8_t>(new uint8_t[LEVEL_MAP_PIXEL_WIDTH * LEVEL_MAP_PIXEL_HEIGHT * BYTES_PER_TEXEL]);
	uint8_t* pixelData = returnData.get();
	for (Vec2 roomPos : roomPositions)
	{
		int channelPos = Get1DChannelPos(minMaxWidth[0], minMaxWidth[1], minMaxHeight[0], minMaxHeight[1], roomPos);

		pixelData[channelPos + 0] = 255u;
		pixelData[channelPos + 1] = 0u;
		pixelData[channelPos + 2] = 0u;
		pixelData[channelPos + 3] = 255u;
	}

	//for (int y = 0; y < sizeY; y++)
	//{
	//	int yPos = y * sizeX;
	//
	//	for (int x = 0; x < sizeX * bytesPerPixel; x += bytesPerPixel)
	//	{
	//		int channelPos = yPos + x;
	//
	//		float xF = (float)x / (float)sizeX;
	//		float yF = (float)y / (float)sizeY;
	//
	//		
	//		pixelData[channelPos + 0 ] = 255;
	//		pixelData[channelPos + 1 ] = 0;
	//		pixelData[channelPos + 2 ] = 0;
	//		pixelData[channelPos + 3 ] = 255;
	//	}
	//}

	return returnData;
}


void LevelHandler::LoadFloors()
{
	const uint blacklistCount = 1;
	string blacklist[blacklistCount] =
	{
		"Hubby"
	};

	for (const auto& item : std::filesystem::directory_iterator(DIR_LEVELS))
	{
		if (item.path().extension() == ".png")
		{
			m_resources.Add(make_shared<LevelResource>());

			shared_ptr<LevelResource>& r = m_resources.Back();
			string s = item.path().filename().string();
			Renderer::LoadLevel(r.get(), s);

			bool blacklisted = false;
			for (uint i = 0; i < blacklistCount; i++)
			{
				if (r->name == blacklist[i])
				{
					blacklisted = true;
				}
			}

			if (!blacklisted)
			{
				m_resourceIndices[r->exits.Size() - 1].Add(m_resources.Size() - 1);
			}
		}
	}
}

void LevelHandler::GenerateFloor(LevelFloor* outFloor, FloorParameters fParams, EnvironmentalizeParameters eParams)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};
	f.startRoom = 0;
	f.startPosition = Vec3( 0, 0, 0 );

	FloorPrimer primer =
	{
		{ fParams.seed },
		{},
		{}
	};

	

	// Bogo-ing

	bool success = false;
	uint attempts = 0;

	float degToRad = cs::c_pi / 180.0f;
	float dotThreshold = std::cosf(5.0f * degToRad);
	
	do
	{
		attempts++;
		CreateNodes(primer, fParams.roomCount, fParams.pushSteps);

		success = TryConnecting(primer, dotThreshold);
		if (!success)
		{
			continue;
		}

		success = TryLeveling(primer, true, 3);
		if (!success)
		{
			continue;
		}

		success = TryFinalizing(primer);
	} 
	while (!success);

	LOG("Generated map network. Attempts: %i", (int)attempts);



	// Create level objects

	float positionModifier = 1.0f;

	for (const TunnelPrimerNetwork& network : primer.networks)
	{
		if (network.merged)
		{
			continue;
		}

		for (const TunnelPrimer& tunnel : network.tunnels)
		{
			f.tunnels.Add({ tunnel.start, tunnel.end });
		}
	}

	for (uint room = 0; room < (uint)primer.rooms.Size(); room++)
	{
		const RoomPrimer& p = primer.rooms[room];

		f.rooms.Add({});
		Level& l = f.rooms.Back();

		l.position = Vec3(p.position.x, 0, p.position.y) * positionModifier * (BM_MAX_SIZE / BM_PIXELS_PER_UNIT);
		l.rotation = Quaternion::GetEuler(0.0f, p.angleOffset, 0.0f);
		l.resource = m_resources[p.levelIndex].get();

		for (uint i = 0; i < (uint)p.connections.Size(); i++)
		{
			int target = p.connections[(i + p.connections.Size() - p.connectionOffset) % p.connections.Size()];

			Mat2 rotMatrix = Mat::rotation2(-p.angleOffset);

			Vec2 exitPixelPosition = l.resource->exits[i].position - Vec2((float)l.resource->pixelWidth, (float)l.resource->pixelHeight) * 0.5f;
			Vec2 exitPixelDirection = l.resource->exits[i].direction;
			exitPixelPosition.y *= -1;
			float width = l.resource->exits[i].width * BM_PIXEL_SIZE;

			Vec2 exitPosition = rotMatrix * exitPixelPosition * BM_PIXEL_SIZE;
			Vec2 exitDirection = rotMatrix * exitPixelDirection;

			Vec2 exitRelativePosition = exitPosition - exitDirection * TUNNEL_SPAWN_DISTANCE;

			Vec3 exitPosition3 = l.position + Vec3(exitPosition.x, 0.0f, exitPosition.y);
			Vec3 exitDirection3 = Vec3(exitDirection.x, 0.0f, exitDirection.y);

			if (target < 0)
			{
				l.connections.Add({ target, 0, 0, exitPosition3, exitDirection3, width });

				if (target == -1)
				{
					f.startPosition = exitPosition3 - exitDirection3 * TUNNEL_SPAWN_DISTANCE;
					f.startRoom = room;
				}

				continue;
			}

			for (uint j = 0; j < (uint)f.tunnels.Size(); j++)
			{
				LevelTunnel& t = f.tunnels[j];
				if (room == t.startRoom && target == (int)t.endRoom)
				{
					t.exits[0] = l.resource->exits[i];
					t.positions[0] = exitPosition3;
					t.directions[0] = exitDirection3;
					l.connections.Add({ target, j, 0, exitPosition3, exitDirection3, width });
					break;
				}
				if (room == t.endRoom && target == (int)t.startRoom)
				{
					t.exits[1] = l.resource->exits[i];
					t.positions[1] = exitPosition3;
					t.directions[1] = exitDirection3;
					l.connections.Add({ target, j, 1, exitPosition3, exitDirection3, width });
					break;
				}
			}
		}

		Environmentalize(l, eParams);
	}
}

void LevelHandler::GenerateTestFloor(LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	// Add a level
	AddLevelName(f, "DuoRoomOne");
	Environmentalize(f.rooms.Back(), params);
}

void LevelHandler::GenerateHubby(LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};
	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);


	// Add a level
	AddLevelName(f, "Hubby");
	Environmentalize(f.rooms.Back(), params);
}

void LevelHandler::Environmentalize(Level& l, EnvironmentalizeParameters parameters)
{
	cs::NoiseSimplex simplexerSpawn(parameters.spawnSeed);
	cs::NoiseSimplex simplexerScale(parameters.scaleSeed);
	cs::NoiseSimplex simplexerRotate(parameters.rotationSeed);
	cs::NoiseSimplex simplexerDiversity(parameters.diversitySeed);

	auto sample = [&](int x, int y)
	{
		if (x < 0 || y < 0 || x >= l.resource->pixelWidth || y >= l.resource->pixelHeight)
		{
			return LevelPixel{ LevelPixelFlagImpassable, 1.0f };
		}

		return l.resource->bitmap[x + l.resource->pixelWidth * y];
	};

	float pushOutFactor = 1.0f;
	float radiusFactor = std::sqrtf(2.0f) * pushOutFactor;
	int maxSize = std::max(l.resource->pixelWidth, l.resource->pixelHeight);
	int extraRadius = (int)((radiusFactor - 1.0f) * 0.5f * maxSize);

	float distanceBoundary = maxSize * 0.5f + extraRadius - 3.0f * BM_PIXELS_PER_UNIT;
	distanceBoundary *= distanceBoundary;

	Vec2 center(l.resource->pixelWidth * 0.5f, l.resource->pixelHeight * 0.5f);
	
	for (int x = 0 - extraRadius; x < (int)l.resource->pixelWidth + extraRadius; x++)
	{
		for (int y = 0 - extraRadius; y < (int)l.resource->pixelHeight + extraRadius; y++)
		{
			if ((Vec2(x, y) - center).LengthSq() > distanceBoundary)
			{
				continue;
			}

			Vec3 newPosition = l.position + l.rotation * Vec3(x * BM_PIXEL_SIZE - l.resource->worldWidth * 0.5f, -0.2f, -y * BM_PIXEL_SIZE + l.resource->worldHeight * 0.5f);

			LevelPixelFlag current = sample(x, y).flags;
			int xP = cs::iclamp(x + parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelWidth - 1);
			int xM = cs::iclamp(x - parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelWidth - 1);
			int yP = cs::iclamp(y + parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelHeight - 1);
			int yM = cs::iclamp(y - parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelHeight - 1);
			LevelPixelFlag xPF = sample(xP, y).flags;
			LevelPixelFlag xMF = sample(xM, y).flags;
			LevelPixelFlag yPF = sample(x, yP).flags;
			LevelPixelFlag yMF = sample(x, yM).flags;
			bool edgeStones = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));


			float noiseVal = simplexerSpawn.Gen2D(x*parameters.xMult, y * parameters.yMult);
			float rotateVal = (cs::c_pi * 2) * simplexerRotate.Gen2D(x * parameters.xMult, y * parameters.yMult)*parameters.rotateMult;
			float scaleVal = parameters.scaleBase + simplexerScale.Gen2D(x * parameters.xMult, y * parameters.yMult) * parameters.scaleMult;
			float diversityVal = simplexerDiversity.Gen2D(x * parameters.xMult, y * parameters.yMult);

			Mat4 stoneMatrix =
				Mat::translation3(newPosition) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierStones,0.2f,2.0f));

			Mat4 treeMatrix =
				Mat::translation3(newPosition) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3( cs::fclamp(scaleVal * parameters.scaleMultiplierTrees, 0.05f, 1.0f));

			Mat4 trunkMatrix =
				Mat::translation3(newPosition) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3( cs::fclamp(scaleVal * parameters.scaleMultiplierTrees*0.25f,0.01, 1.0f));

			if ((sample(x, y).flags & LevelPixelFlagTerrainInner & ~LevelPixelFlagTerrainOuter))
			{

				xP = cs::iclamp(x + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				xM = cs::iclamp(x - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				yP = cs::iclamp(y + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				yM = cs::iclamp(y - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				xPF = sample(xP, y).flags;
				xMF = sample(xM, y).flags;
				yPF = sample(x, yP).flags;
				yMF = sample(x, yM).flags;
				bool edgeTree = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));


				Mat4 foliageMatrix =
					Mat::translation3(newPosition) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3(scaleVal * parameters.scaleMultiplierFoliage);

				if (noiseVal < parameters.densityUnwalkableInner)
				{
					if (diversityVal < 0.33f && !edgeStones)
					{
						l.instances[LevelAssetStone1].Add(stoneMatrix);
					}
					else if (diversityVal < 0.4f && !edgeTree)
					{
						l.instances[LevelAssetTree1].Add(treeMatrix);
					}
					else if (diversityVal < 0.45f && !edgeTree)
					{
						l.instances[LevelAssetTree2].Add(treeMatrix);
					}
					else if (diversityVal < 0.5f && !edgeTree)
					{
						l.instances[LevelAssetTree3].Add(treeMatrix);
					}
					else if (diversityVal < 0.66f && !edgeStones)
					{
						l.instances[LevelAssetStone2].Add(stoneMatrix);
					}
					else 
					{
						l.instances[LevelAssetBush1].Add(foliageMatrix);
					}
				}
			}
			else if ((sample(x, y).flags & LevelPixelFlagImpassable))
			{
				Mat4 foliageMatrixHuge =
					Mat::translation3( newPosition ) *
					Mat::rotation3( cs::c_pi * -0.5f, rotateVal, 0.0f ) *
					Mat::scale3( cs::fclamp( scaleVal * parameters.scaleMultiplierFoliage * 2.5, 0.1f, 2.2f ) );

				Mat4 foliageMatrix =
					Mat::translation3(newPosition) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3( cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * 1.5f,0.05f, 2.0f));
				Mat4 foliageMatrix2 =
					Mat::translation3( newPosition ) *
					Mat::rotation3( cs::c_pi * -0.5f, rotateVal, 0.0f ) *
					Mat::scale3( cs::fclamp( scaleVal * parameters.scaleMultiplierFoliage * 1.2f, 0.1, 1.5f));
				xP = cs::iclamp(x + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
				xM = cs::iclamp(x - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
				yP = cs::iclamp(y + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
				yM = cs::iclamp(y - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
				xPF = sample(xP, y).flags;
				xMF = sample(xM, y).flags;
				yPF = sample(x, yP).flags;
				yMF = sample(x, yM).flags;
				bool edgeTrunk = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));

				xP = cs::iclamp(x + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				xM = cs::iclamp(x - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				yP = cs::iclamp(y + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				yM = cs::iclamp(y - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				xPF = sample(xP, y).flags;
				xMF = sample(xM, y).flags;
				yPF = sample(x, yP).flags;
				yMF = sample(x, yM).flags;
				bool edgeTree = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));
				if (noiseVal < parameters.densityUnwalkableOuter)
				{


					if (diversityVal < 0.1f && !edgeStones)
					{
						l.instances[LevelAssetStone1].Add(stoneMatrix);
					}
					else if (diversityVal < 0.2f && !edgeStones)
					{
						l.instances[LevelAssetStone2].Add(stoneMatrix);
					}
					else if (diversityVal < 0.4f && !edgeTree)
					{
						l.instances[LevelAssetTree1].Add(treeMatrix);
					}
					else if (diversityVal < 0.6f)
					{
						l.instances[LevelAssetBush1].Add(foliageMatrix);
					}
					else if (diversityVal < 0.7f && !edgeTree)
					{
						l.instances[LevelAssetTree2].Add(treeMatrix);
					}
					else if (diversityVal < 0.8f && !edgeTree)
					{
						l.instances[LevelAssetTree3].Add(treeMatrix);
					}
					else if (diversityVal < 0.9f && !edgeTrunk)
					{
						l.instances[LevelAssetBigTrunk1].Add(trunkMatrix);
					}
					else if (!edgeTrunk)
					{
						l.instances[LevelAssetBigTrunk2].Add(trunkMatrix);
					}
				}

				if (noiseVal < parameters.densityUnwalkableOuter * 1.5f)
				{
					if (diversityVal < 0.5f && !edgeStones)
					{
						l.instances[LevelAssetStone1].Add( stoneMatrix );
					}
					else if (diversityVal < 0.8f && !edgeStones)
					{
						l.instances[LevelAssetStone2].Add( stoneMatrix );
					}
				}

				if (noiseVal < parameters.densityWalkable * 0.5f && !edgeTree)
				{
					l.instances[LevelAssetBush1].Add( foliageMatrixHuge );
				}
				else if (noiseVal < parameters.densityWalkable * 0.5f && !edgeStones)
				{
					l.instances[LevelAssetBush1].Add(foliageMatrix);
				}
				else if (noiseVal < parameters.densityWalkable * 0.5f)
				{
					l.instances[LevelAssetBush1].Add(foliageMatrix2);
				}
			}
			else if ((sample(x, y).density != 0))
			{
				float density = 1.0f - sample(x, y).density;
				Mat4 foliageMatrix =
					Mat::translation3(newPosition) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * (density * (1.0f - parameters.scaleEffectDensity)),0.1f, 2.0f ));

				if (density > parameters.minDensity && noiseVal < parameters.densityWalkable)
				{
					l.instances[LevelAssetBush1].Add(foliageMatrix);
				}
			}

			//if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagTerrainInner & ~LevelPixelFlagTerrainOuter))
			//{
			//	if (r.Get( 10 ) == 0)
			//	{
			//		Mat4 instanceMatrix =
			//			Mat::translation3( offset + Vec3( -x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE ) ) *
			//			Mat::rotation3( cs::c_pi * -0.5f, r.Getf( 0, cs::c_pi * 2 ), 0.0f ) *
			//			Mat::scale3( 0.1f );
			//		Mat4 stoneMatrix =
			//			Mat::translation3( offset + Vec3( -x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE ) ) *
			//			Mat::rotation3( cs::c_pi * -0.5f, r.Getf( 0, cs::c_pi * 2 ), 0.0f ) *
			//			Mat::scale3( 0.2f + r.Getf() * 0.2f );
			//		Mat4 banana =
			//			Mat::translation3( offset + Vec3( -x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE ) ) *
			//			Mat::rotation3( cs::c_pi * -0.5f, r.Getf( 0, cs::c_pi * 2 ), 0.0f ) *
			//			Mat::scale3( 0.25f + r.Getf() * 0.25f );
			//		//l.instances[LevelAssetBush1].Add(instanceMatrix);
			//		int rand = r.Get( 100 );
			//		if (rand < 25)
			//		{
			//			l.instances[LevelAssetMediumStone2].Add( stoneMatrix );
			//		}
			//		else if (rand < 75)
			//		{
			//			l.instances[LevelAssetBush1].Add( banana );
			//		}
			//		else
			//		{
			//			l.instances[LevelAssetMediumStone1].Add( stoneMatrix );
			//		}
			//	}
			//}
			//else if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagImpassable) && r.Get(50) == 0)
			//{
			//	Mat4 instanceMatrix = 
			//		Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE)) * 
			//		Mat::rotation3(cs::c_pi * -0.5f, r.Getf(0, cs::c_pi * 2), 0.0f) * 
			//		Mat::scale3(0.1f, 0.1f, 0.1f );
			//	Mat4 trunkMatrix =
			//		Mat::translation3( offset + Vec3( -x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE ) ) *
			//		Mat::rotation3( cs::c_pi * -0.5f, r.Getf( 0, cs::c_pi * 2 ), 0.0f ) *
			//		Mat::scale3( 0.03f, 0.03f, 0.1f );
			//	int rand = r.Get( 100 );

			//	if (rand < 25)
			//	{
			//		l.instances[LevelAssetMediumStone1].Add( instanceMatrix );
			//	}
			//	else if (rand < 30)
			//	{
			//		l.instances[LevelAssetMediumTree3].Add( instanceMatrix );
			//	}
			//	else if (rand < 40)
			//	{
			//		l.instances[LevelAssetMediumTree2].Add( instanceMatrix );
			//	}
			//	else if (rand < 50)
			//	{
			//		l.instances[LevelAssetMediumBigTrunk2].Add( instanceMatrix );
			//	}
			//	else if (rand < 60)
			//	{
			//		l.instances[LevelAssetMediumTree1].Add( instanceMatrix );
			//	}
			//	else if (rand < 75)
			//	{
			//		l.instances[LevelAssetMediumBigTrunk1].Add( trunkMatrix );
			//	}
			//	else
			//	{
			//		l.instances[LevelAssetMediumBigTrunk2].Add( trunkMatrix );
			//	}
			//}
			//else if ((l.resource->bitmap[x + l.resource->pixelWidth * y].density < (float)r.Get( 100 )/100.0f) && (l.resource->bitmap[x + l.resource->pixelWidth * y].density != 0) && r.Get( 20 ) == 0)
			//{
			//	Mat4 instanceMatrix = Mat::translation3( offset + Vec3( -x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE ) ) * Mat::rotation3( cs::c_pi * -0.5f, r.Getf( 0, cs::c_pi * 2 ), 0.0f ) * Mat::scale3( 0.1f+ (0.3f-(l.resource->bitmap[x + l.resource->pixelWidth * y].density*0.3f)));
			//	l.instances[LevelAssetBush1].Add( instanceMatrix );	
			//}
		
		}
	}
}

void LevelHandler::AddLevelName(LevelFloor& f, string name)
{
	for (int i = 0; i < m_resources.Size(); i++)
	{
		if (m_resources[i]->name == name)
		{
			f.rooms.Add({});
			f.rooms.Back().resource = m_resources[i].get();
		}
	}
}

int LevelHandler::GetLevelByName(string name)
{
	for (int i = 0; i < m_resources.Size(); i++)
	{
		if (m_resources[i]->name == name)
		{
			return i;
		}
	}

	return -1;
}

void LevelHandler::CreateNodes(FloorPrimer& f, uint roomCount, uint pushSteps)
{
	f.rooms.Clear(false);
	
	// Generate
	for (uint i = 0; i < roomCount; i++)
	{
		f.rooms.Add({ {}, { f.r.Getf(-5.0f, 5.0f), f.r.Getf(-5.0f, 5.0f) }, {} });
	}

	// Push apart
	for (int repetitions = 0; repetitions < (int)pushSteps; repetitions++)
	{
		for (uint i = 0; i < roomCount; i++)
		{
			f.rooms[i].push = { 0, 0 };

			for (uint j = 0; j < roomCount; j++)
			{
				if (i == j)
				{
					continue;
				}

				Vec2 direction = f.rooms[j].position - f.rooms[i].position;
				f.rooms[i].push += direction.Normalized() / cs::fmax(direction.LengthSq(), 0.3f);
			}
		}

		for (uint i = 0; i < roomCount; i++)
		{
			f.rooms[i].position += f.rooms[i].push * -0.5f;
		}
	}
}

bool LevelHandler::TryConnecting(FloorPrimer& f, float dotThreshold)
{
	f.networks.Clear();

	// Link room primers
	cs::List<uint> connectionCounts;
	cs::List<uint> loose;
	cs::List<uint> looseNext;

	for (uint i = 0; i < (uint)f.rooms.Size(); i++)
	{
		loose.Add(i);
		connectionCounts.Add(0);

		f.rooms[i].networkIndex = -1;
	}

	auto pred = [&](uint a, uint b) { return f.rooms[a].position.x < f.rooms[b].position.x; };
	auto clear = [&](uint start, uint end)
	{
		if (connectionCounts[start] > 2 || connectionCounts[end] > 2)
		{
			return false;
		}

		Vec2 ori = f.rooms[start].position;
		Vec2 dir = f.rooms[end].position - ori;
		Vec2 ndir = dir.Normalized();

		if (f.rooms[start].networkIndex >= 0)
		{
			TunnelPrimerNetwork& startNetwork = f.networks[f.rooms[start].networkIndex];
			for (int i = 0; i < startNetwork.tunnels.Size(); i++)
			{
				if (startNetwork.tunnels[i].start != start && startNetwork.tunnels[i].end != start)
				{
					continue;
				}

				uint other = startNetwork.tunnels[i].start == start ?
					startNetwork.tunnels[i].end :
					startNetwork.tunnels[i].start;

				Vec2 relativeDir = (f.rooms[other].position - ori).Normalized();

				if (relativeDir * ndir > dotThreshold)
				{
					return false;
				}
			}
		}

		if (f.rooms[end].networkIndex >= 0)
		{
			TunnelPrimerNetwork& endNetwork = f.networks[f.rooms[end].networkIndex];
			for (int i = 0; i < endNetwork.tunnels.Size(); i++)
			{
				if (endNetwork.tunnels[i].start != end && endNetwork.tunnels[i].end != end)
				{
					continue;
				}

				uint other = endNetwork.tunnels[i].start == end ?
					endNetwork.tunnels[i].end :
					endNetwork.tunnels[i].start;

				Vec2 relativeDir = (f.rooms[end].position - f.rooms[other].position).Normalized();

				if (relativeDir * ndir > dotThreshold)
				{
					return false;
				}
			}
		}

		for (const TunnelPrimerNetwork& network : f.networks)
		{
			for (const TunnelPrimer& tunnel : network.tunnels)
			{
				if (start == tunnel.start || start == tunnel.end || end == tunnel.start || end == tunnel.end)
				{
					if (start == tunnel.start && end == tunnel.end || start == tunnel.end && end == tunnel.start)
					{
						return false;
					}

					continue;
				}

				Vec2 otherOri = f.rooms[tunnel.start].position;
				Vec2 otherDir = f.rooms[tunnel.end].position - otherOri;

				if (Intersect(ori, ori + dir, otherOri, otherOri + otherDir))
				{
					return false;
				}
			}
		}

		return true;
	};

	int iterations = 0;
	int networkCount = 0;

	do
	{
		while (loose.Size() > 0)
		{
			bool connected = false;

			uint currentIndex = loose.Back();
			Vec2 current = f.rooms[currentIndex].position;

			float previousClosest = 0.0f;

			if (connectionCounts[currentIndex] > 2)
			{
				loose.Pop();
				continue;
			}

			for (uint attempt = 0; attempt < 4; attempt++)
			{

				int closest = 0;
				float closestDistance = INFINITY;

				for (int i = f.rooms.Size() - 1; i >= 0; i--)
				{
					if ((uint)i == currentIndex)
					{
						continue;
					}

					Vec2 target = f.rooms[i].position;
					float distance = (current - target).LengthSq();
					if (distance < closestDistance && distance > previousClosest + 0.0001f)
					{
						closest = i;
						closestDistance = distance;
					}
				}

				if (clear(currentIndex, closest))
				{
					if (f.rooms[closest].networkIndex >= 0)
					{
						f.networks[f.rooms[closest].networkIndex].tunnels.Add({ (uint)closest, currentIndex });

						if (f.rooms[currentIndex].networkIndex >= 0 && f.rooms[currentIndex].networkIndex != f.rooms[closest].networkIndex)
						{
							f.networks[f.rooms[closest].networkIndex].tunnels.MassAdd(
								f.networks[f.rooms[currentIndex].networkIndex].tunnels, true);

							f.networks[f.rooms[currentIndex].networkIndex].merged = true;

							uint tempIndex = f.rooms[currentIndex].networkIndex;
							for (const TunnelPrimer& t : f.networks[tempIndex].tunnels)
							{
								f.rooms[t.start].networkIndex = f.rooms[closest].networkIndex;
								f.rooms[t.end].networkIndex = f.rooms[closest].networkIndex;
							}

							f.rooms[currentIndex].networkIndex = f.rooms[closest].networkIndex;
							f.networks[tempIndex].tunnels.Clear(false);

							networkCount--;
						}
						else
						{
							f.rooms[currentIndex].networkIndex = f.rooms[closest].networkIndex;
						}
					}
					else
					{
						f.networks.Add({ { { currentIndex, (uint)closest } } });
						f.rooms[currentIndex].networkIndex = f.networks.Size() - 1;
						f.rooms[closest].networkIndex = f.networks.Size() - 1;

						networkCount++;
					}

					connectionCounts[currentIndex]++;
					connectionCounts[closest]++;

					connected = true;

					break;
				}

				previousClosest = closestDistance;
			}

			looseNext.Add(loose.Pop());
		}

		loose.MassAdd(looseNext);
		looseNext.Clear(false);
		iterations++;
	} 
	while (loose.Size() > 0 && networkCount > 1 && iterations < 4);



	// BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO BOGO

	if (networkCount > 1)
	{
		return false;
	}

	for (int i = 0; i < connectionCounts.Size(); i++)
	{
		if (connectionCounts[i] == 0)
		{
			return false; // MAXIMUM BOGO
		}
	}

	for (uint i = 0; i < (uint)f.networks.Size(); i++)
	{
		if (!f.networks[i].merged)
		{
			for (const TunnelPrimer& t : f.networks[i].tunnels)
			{
				f.rooms[t.start].connections.Add(t.end);
				f.rooms[t.end].connections.Add(t.start);
			}
		}
	}

	return true;
}

bool LevelHandler::TryLeveling(FloorPrimer& f, bool repeats, uint roomAttempts)
{
	//unique_ptr<cs::List<uint>[]> levelRefs(new cs::List<uint>[m_resources.Size()]);

	cs::List<uint> currentRoomIndices;
	cs::List<shared_ptr<LevelResource>*> currentRooms;

	AngleRooms(f);

	for (uint i = 0; i < (uint)f.rooms.Size(); i++)
	{
		RoomPrimer& r = f.rooms[i];

		currentRoomIndices.Clear(false);
		currentRooms.Clear(false);

		const cs::List<uint>& source = m_resourceIndices[r.connections.Size() - 1];

		float bestDeviation = INFINITY;
		int bestIndex = 0;

		for (uint j = 0; j < roomAttempts; j++)
		{
			uint newIndex = source[f.r.Get(source.Size())];
			bool dupe = false;

			for (uint k = 0; k < (uint)currentRoomIndices.Size(); k++)
			{
				if (currentRoomIndices[k] == newIndex)
				{
					dupe = true;
					break;
				}
			}

			if (dupe)
			{
				j--;
				continue;
			}

			if (r.connections.Size() == 1)
			{
				bestDeviation = 0.0f;
				bestIndex = (int)newIndex;
				break;
			}

			float deviation = EvaluateDeviation(r, m_resources[newIndex].get());
			if (deviation < bestDeviation)
			{
				bestDeviation = deviation;
				bestIndex = (int)newIndex;
			}
		}

		r.levelIndex = bestIndex;
		EvaluateRoom(f, i);
	}

	return true;
}

bool LevelHandler::TryFinalizing(FloorPrimer& f)
{
	cs::List<uint> leaves;
	cs::List<uint> doubles;
	for (uint i = 0; i < (uint)f.rooms.Size(); i++)
	{
		if (f.rooms[i].connections.Size() == 1)
		{
			leaves.Add(i);
		}

		if (f.rooms[i].connections.Size() == 2)
		{
			doubles.Add(i);
		}
	}

	if (leaves.Size() == 0 || leaves.Size() + doubles.Size() < 3)
	{
		return false;
	}

	uint essenceBloomIndex = f.r.GetUnsigned(leaves.Size());
	f.rooms[leaves[essenceBloomIndex]].essenceBloom = true;
	leaves.Remove(essenceBloomIndex);

	cs::List<int> assigns = { -1, -2 };
	while (assigns.Size() > 0 && leaves.Size() > 0)
	{
		// Convert from leaf to entrance/exit
		const cs::List<uint>& source = m_resourceIndices[1];
		uint newIndex = source[f.r.Get(source.Size())];

		uint target = leaves.Pop();
		f.rooms[target].levelIndex = (int)newIndex;
		f.rooms[target].connections.Add(assigns.Pop());

		AngleRoom(f, f.rooms[target]);
		EvaluateRoom(f, target);
	}

	//while (assigns.Size() > 0 && doubles.Size() > 0)
	//{
	//	// Convert from 2-spoke connector to triple with entrance/exit
	//}

	if (assigns.Size() > 0)
	{
		return false;
	}

	return true;
}

void LevelHandler::AngleRoom(FloorPrimer& f, RoomPrimer& r)
{
	r.angles.Clear(false);
	for (uint i = 0u; i < (uint)r.connections.Size(); i++)
	{
		Vec2 position;

		if (r.connections[i] < 0)
		{
			Vec2 sum;
			for (uint j = 0; j < (uint)r.connections.Size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				sum += f.rooms[r.connections[j]].position - r.position;
			}

			position = r.position - sum.Normalized();
		}
		else
		{
			position = f.rooms[r.connections[i]].position;
		}

		Vec2 direction = position - r.position;
		r.angles.Add(cs::fwrap(std::atan2f(-direction.y, direction.x), 0.0f, 2 * cs::c_pi));
	}

	struct AngleSorter
	{
		float angle;
		uint index;
	};

	cs::List<AngleSorter> sorter;
	for (uint i = 0u; i < (uint)r.angles.Size(); i++)
	{
		sorter.Add({ r.angles[i], i });
	}

	std::sort(
		&sorter.Front(),
		&sorter.Back() + 1,
		[](const AngleSorter& a, const AngleSorter& b) { return a.angle < b.angle; });

	for (uint i = 0u; i < (uint)r.angles.Size(); i++)
	{
		r.connections.Add(r.connections[sorter[i].index]);
		r.angles[i] = sorter[i].angle;
	}

	for (uint i = 0u; i < (uint)r.angles.Size(); i++)
	{
		r.connections.Remove(0);
	}
}

void LevelHandler::AngleRooms(FloorPrimer& f)
{
	for (RoomPrimer& r : f.rooms)
	{
		AngleRoom(f, r);
	}
}

void LevelHandler::EvaluateRoom(FloorPrimer& f, uint index)
{
	RoomPrimer& r = f.rooms[index];

	// Working values
	static cs::List<float> angles;
	static cs::List<float> offsetDeviations;

	if (r.levelIndex >= 0)
	{
		const LevelResource* resource = m_resources[r.levelIndex].get();

		r.angleDeviations.Clear(false);
		r.angleDeviationScore = INFINITY;
		r.angleOffset = 0.0f;
		r.connectionOffset = 0u;

		offsetDeviations.Clear(false);

		if (r.levelIndex == GetLevelByName("Test3-2"))
		{
			int a = 0;
		}

		// Try for every permutation of offsets between level exits and real connections
		for (uint i = 0u; i < (uint)r.angles.Size(); i++)
		{
			angles.Clear(false);
			float average = 0.0f;

			for (uint j = 0u; j < (uint)r.angles.Size(); j++)
			{
				const LevelExit& exit = resource->exits[(j + i) % r.angles.Size()];

				float myAngle = r.angles[j];
				float exitAngle = exit.angle;

				angles.Add(myAngle - exitAngle);

				float modifier = 0.0f;
				if (angles.Back() - average < -cs::c_pi)
				{
					modifier += cs::c_pi * 2;
				}
				else if (angles.Back() - average > cs::c_pi)
				{
					modifier -= cs::c_pi * 2;
				}

				average += angles.Back() + modifier;
			}

			average = cs::fwrap(average / r.angles.Size(), 0.0f, 2 * cs::c_pi);

			float deviationScore = 0.0f;
			for (uint j = 0u; j < (uint)r.angles.Size(); j++)
			{
				offsetDeviations.Add(angles[j] - average);
				deviationScore += std::fabs(cs::fwrap(offsetDeviations.Back(), -cs::c_pi, cs::c_pi));
			}

			if (deviationScore < r.angleDeviationScore)
			{
				r.angleDeviationScore = deviationScore;
				r.angleOffset = average;
				r.connectionOffset = i;

				r.angleDeviations.Clear(false);
				r.angleDeviations.MassAdd(offsetDeviations, true);
			}
		}
	}
}

float LevelHandler::EvaluateDeviation(RoomPrimer& r, const LevelResource* level)
{
	// Working values
	static cs::List<float> angles;

	float result = 0.0f;

	result = INFINITY;

	// Try for every permutation of offsets between level exits and real connections
	for (uint i = 0u; i < (uint)r.angles.Size(); i++)
	{
		angles.Clear(false);
		float average = 0.0f;

		for (uint j = 0u; j < (uint)r.angles.Size(); j++)
		{
			const LevelExit& exit = level->exits[(j + i) % r.angles.Size()];

			float myAngle = r.angles[j];
			float exitAngle = exit.angle;

			angles.Add(myAngle - exitAngle);

			float modifier = 0.0f;
			if (angles.Back() - average < -cs::c_pi)
			{
				modifier += cs::c_pi * 2;
			}
			else if (angles.Back() - average > cs::c_pi)
			{
				modifier -= cs::c_pi * 2;
			}

			average += angles.Back() + modifier;
		}

		average = cs::fwrap(average / r.angles.Size(), 0.0f, 2 * cs::c_pi);

		float deviationScore = 0.0f;
		for (uint j = 0u; j < (uint)r.angles.Size(); j++)
		{
			deviationScore += std::fabs(cs::fwrap(angles[j] - average, -cs::c_pi, cs::c_pi));
		}

		if (deviationScore < result)
		{
			result = deviationScore;
		}
	}

	return result;
}


