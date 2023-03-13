#include "core.h"
#include "LevelHandler.h"
#include "LevelImporter.h"
#include "Renderer.h"

// Includes to make creation of floor texture possible in constructor
#include "Resources.h"
#include "Renderer.h"

#include <filesystem>

constexpr uint MINIMAP_PIXEL_SIZE = 400u;
constexpr uint BYTES_PER_TEXEL = 4u;
constexpr uint DATA_SIZE = MINIMAP_PIXEL_SIZE * MINIMAP_PIXEL_SIZE * BYTES_PER_TEXEL;

// All calculated from positions are shrinked by certain amount to fit better in texture.
constexpr float UV_POS_PADDING_FACTOR = 0.5f;


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
	m_floorMinimapGUI = shared_ptr<GUI>(new GUI());

	Vec2 minimapPosition = Vec2(-0.95f, 0.3f);
	Vec2 minimapSize = Vec2(0.4f * 0.9f, 0.4f * 1.6f);

	// Add the map texture
	shared_ptr<GUIElement> minimapElement = m_floorMinimapGUI->AddGUIElement(minimapPosition, minimapSize, nullptr, nullptr);
	minimapElement->colorTint = cs::Color3f(0xFFFFFF); // This is the base color of the map.
	minimapElement->alpha = 0.8f;
	minimapElement->intData = Point4(0, 0, 1, 0); // Makes it zoom in around a given uv point.

	TextureResource* minimapTexture = Resources::Get().CreateTextureUnorm(
		Renderer::Get().GetRenderCore(),
		"FloorMinimapGUI",
		nullptr,
		MINIMAP_PIXEL_SIZE,
		MINIMAP_PIXEL_SIZE,
		true
	);

	minimapElement->firstTexture = minimapTexture;
	minimapElement->secondTexture = Resources::Get().GetTexture("HudMask2.png");

	// Add the overlay
	shared_ptr<GUIElement> minimapOverlayElement = m_floorMinimapGUI->AddGUIElement(minimapPosition, minimapSize, nullptr, nullptr );
	minimapOverlayElement->colorTint = cs::Color3f( 0.9f,0.9f,0.9f ); // This is the base color of the map overlay.
	minimapOverlayElement->alpha = 1.0f;
	minimapOverlayElement->intData = Point4( 0, 0, 0, 0 );
	minimapOverlayElement->firstTexture = Resources::Get().GetTexture( "MinimapOverlay.png" );
	minimapOverlayElement->secondTexture = Resources::Get().GetTexture( "MiniMapOverlayMask.png" );

	
	minimapElement->uiRenderable->enabled = false;
	m_floorMinimapGUIElement = minimapElement;
	m_floorMinimapOverlayGUIElement = minimapOverlayElement;

	m_minimapBackgroundColor = cs::Color3(0x282928);
	m_minimapNodeColor = cs::Color3(0xa0faa0);
	m_minimapNodeExitColor = cs::Color3(0xa0d9fa);
	m_minimapConnectionColor = cs::Color3(0xd2fad2);

	m_minimapTexelSize = 1.0f / MINIMAP_PIXEL_SIZE;
	m_nodePixelRadius = 10u;

	// Allocate data for the minimap texture.
	m_minimapTextureData = shared_ptr<uint8_t>(new uint8_t[DATA_SIZE](0));
}

int LevelHandler::Get1DPixelPosFromWorld(Vec2 position)
{
	Vec2 uvPos = GetFloorUVFromPos(position);

	return GetSafe1DPixelPosFromUV(uvPos);
}

int LevelHandler::GetSafe1DPixelPosFromUV(Vec2 uv)
{
	// Simple UV mapping of coordinates to pixel pos.
	int yOffset = cs::floor(uv.y * MINIMAP_PIXEL_SIZE) * MINIMAP_PIXEL_SIZE;
	int xOffset = cs::floor(uv.x * MINIMAP_PIXEL_SIZE);

	return cs::iclamp((yOffset + xOffset) * BYTES_PER_TEXEL, 0, (int)DATA_SIZE);
}

Vec2 LevelHandler::GetFloorUVFromPos(Vec2 position)
{
	// Default value of 0.5f for special cases.
	float xUVPos = 0.5f;
	float yUVPos = 0.5f;

	if ((m_minimapWorldMaxWidth - m_minimapWorldMinWidth) != 0.0f)
	{
		xUVPos = (position.x - m_minimapWorldMinWidth) / (m_minimapWorldMaxWidth - m_minimapWorldMinWidth);
	}

	if ((m_minimapWorldMaxHeight - m_minimapWorldMinHeight) != 0.0f)
	{
		yUVPos = (position.y - m_minimapWorldMinHeight) / (m_minimapWorldMaxHeight - m_minimapWorldMinHeight);
	}

	Vec2 uvPos = Vec2(xUVPos, yUVPos);
	Vec2 midPoint = Vec2(0.5f, 0.5f);
	// Offset interval to [-0.5, 0.5]. Shrink interval towards 0. Offset back to original interval of [0, 1].
	return (uvPos - midPoint) * UV_POS_PADDING_FACTOR + midPoint;
}

Vec2 LevelHandler::GetRoomFlatenedPos(Vec3 roomPosition)
{
	return Vec2(roomPosition.x, roomPosition.z);
}

void LevelHandler::LoadPixel(uint pixelPos, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t* textureData = m_minimapTextureData.get();
	textureData[pixelPos + 0] = r;
	textureData[pixelPos + 1] = g;
	textureData[pixelPos + 2] = b;
	textureData[pixelPos + 3] = 255u;
}

void LevelHandler::LoadPixel(uint pixelPos, cs::Color3 color)
{
	LoadPixel(pixelPos, color.r, color.g, color.b);
}

void LevelHandler::GenerateFloorImageData(LevelFloor* floorRef)
{
	uint roomExitIndex = UINT_MAX;

	// Save looped rooms for later use when creating nodes.
	cs::List<Vec2> roomPositions = {};
	for (int i = 0; i < floorRef->rooms.Size(); i++)
	{
		Level& level = floorRef->rooms[i];
		for (LevelTunnelRef& connection : level.connections)
		{
			// If the room/level contains a connection that leads to -2, its the exit room.
			if (connection.targetRoom == -2)
			{
				roomExitIndex = roomPositions.Size();
				m_exitRoom = &level;
				break;
			}
		}

		Vec3 roomPos = level.position;
		Vec2 roomFlatPos = GetRoomFlatenedPos(roomPos);

		m_minimapWorldMinHeight = cs::fmin(m_minimapWorldMinHeight, roomFlatPos.y);
		m_minimapWorldMaxHeight = cs::fmax(m_minimapWorldMaxHeight, roomFlatPos.y);

		m_minimapWorldMinWidth = cs::fmin(m_minimapWorldMinWidth, roomFlatPos.x);
		m_minimapWorldMaxWidth = cs::fmax(m_minimapWorldMaxWidth, roomFlatPos.x);

		roomPositions.Add(roomFlatPos);
		LOG_TRACE("%d - Room pos: %.2f %.2f %.2f", i, roomPos.x, roomPos.y, roomPos.z);
	}

	// --------- Start of filling pixel data --------------

	// Fill with background color.
	ClearMinimapTexture(m_minimapBackgroundColor);
	
	// Draw lines between nodes
	for (int i = 0; i < floorRef->tunnels.Size(); i++)
	{
		const LevelTunnel& tunnel = floorRef->tunnels[i];
		//Vec2 posA = GetFloorUVFromPos(GetRoomFlatenedPos(tunnel.positions[0]));
		//Vec2 posB = GetFloorUVFromPos(GetRoomFlatenedPos(tunnel.positions[1]));
		Vec2 posA = GetFloorUVFromPos(GetRoomFlatenedPos(floorRef->rooms[tunnel.startRoom].position));
		Vec2 posB = GetFloorUVFromPos(GetRoomFlatenedPos(floorRef->rooms[tunnel.endRoom].position));

		DrawLine(posA, posB, m_minimapConnectionColor);
	}

	for (int i = 0; i < roomPositions.Size(); i++)
	{
		Vec2 roomUVPos = GetFloorUVFromPos(roomPositions[i]);
		
		cs::Color3 drawColor;
		if (i == roomExitIndex)
		{
			drawColor = m_minimapNodeExitColor;
		}
		else
		{
			drawColor = m_minimapNodeColor;
		}

		DrawNode(roomUVPos, drawColor);
	}
}


void LevelHandler::ResetMinimapValues()
{
	m_currentRoom = nullptr;
	m_previousRoom = nullptr;
	m_exitRoom = nullptr;

	m_minimapWorldMinHeight = FLT_MAX;
	m_minimapWorldMaxHeight = -FLT_MAX;

	m_minimapWorldMinWidth = FLT_MAX;
	m_minimapWorldMaxWidth = -FLT_MAX;
}

void LevelHandler::GenerateNewMinimap(LevelFloor* floorRef)
{
	ResetMinimapValues();

	// After creating floor structure, create its image.
	GenerateFloorImageData(floorRef);
	m_floorMinimapGUIElement->uiRenderable->enabled = true;
	UpdateDirectXTexture();
}

void LevelHandler::UpdateDirectXTexture()
{
	Renderer::UpdateTexture2DData(
		m_floorMinimapGUIElement->firstTexture->texture2D,
		m_minimapTextureData.get(),
		MINIMAP_PIXEL_SIZE,
		MINIMAP_PIXEL_SIZE
	);
}

void LevelHandler::SetFloormapFocusRoom(Level* level)
{
	if (level == nullptr)
	{
		LOG_WARN("Focus room was nullptr. Early return.");
		return;
	}

	m_previousRoom = m_currentRoom;
	m_currentRoom = level;

	Vec2 currentRoomPos = GetFloorUVFromPos(GetRoomFlatenedPos(m_currentRoom->position));
	m_floorMinimapGUIElement->minimapCurrentRoomPos = currentRoomPos;

	// If this sort of check needs to be done at more places, this could be a member variable.
	bool textureIsUpdated = false;

	// Do not change back color if its a special room.
	if (m_previousRoom != m_exitRoom && m_previousRoom != nullptr)
	{
		Vec2 previousRoomPos = GetFloorUVFromPos(GetRoomFlatenedPos(m_previousRoom->position));
		DrawNode(previousRoomPos, m_minimapNodeColor);
		textureIsUpdated |= true;
	}
	
	// Do not change color if its a special room.
	if (m_currentRoom != m_exitRoom)
	{
		DrawNode(currentRoomPos, cs::Color3(0xFFFFFF));
		textureIsUpdated |= true;
	}
	
	if(textureIsUpdated)
		UpdateDirectXTexture();
}

void LevelHandler::MinimapSetEnable(bool enable)
{
	m_floorMinimapGUIElement->uiRenderable->enabled = enable;
	m_floorMinimapOverlayGUIElement->uiRenderable->enabled = enable;
}

void LevelHandler::DrawLine(Vec2 uvPosA, Vec2 uvPosB, cs::Color3 lineColor)
{
	Vec2 uvDirectionBToA = uvPosA - uvPosB;
	float lineLength = uvDirectionBToA.Length();
	uvDirectionBToA.Normalize(); // Make normalized direction after getting length.

	// Calculates one normal (-dy, dx).
	Vec2 uvDirectionNormal = Vec2(-(uvPosA.y - uvPosB.y), uvPosA.x - uvPosB.x).Normalize();

	uint minSteps = cs::ceil(lineLength / m_minimapTexelSize);
	const int halfLineWidth = 2;
	for (int step = 0; step < minSteps; step++)
	{
		Vec2 texelPos = uvPosB + uvDirectionBToA * m_minimapTexelSize * step;

		for (int widthStep = -halfLineWidth; widthStep <= halfLineWidth; widthStep++)
		{
			// Magic number scalar to ensure proper filling of line (no gaps between each width step).
			Vec2 offset = uvDirectionNormal * m_minimapTexelSize * widthStep * 0.7f;
			int pixelPos = GetSafe1DPixelPosFromUV(texelPos + offset);

			LoadPixel(pixelPos, m_minimapConnectionColor);
		}
	}
}

void LevelHandler::DrawNode(Vec2 uvPosNode, cs::Color3 nodeColor)
{
	// Only used so value can be negative without having to cast from uint every time.
	const int radius = m_nodePixelRadius;
	const Vec2 centerTexelOffset = Vec2(m_minimapTexelSize, m_minimapTexelSize) / 2.0f;
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			Vec2 cubeOffset = Vec2(x, y) * m_minimapTexelSize + centerTexelOffset;
			if (cubeOffset.Length() <= (radius * m_minimapTexelSize))
			{
				int pixelPos = GetSafe1DPixelPosFromUV(cubeOffset + uvPosNode);
				LoadPixel(pixelPos, nodeColor);
			}
		}
	}
}

void LevelHandler::ClearMinimapTexture(cs::Color3 clearColor)
{
	for (int i = 0; i < DATA_SIZE; i += 4)
	{
		LoadPixel(i, clearColor);
	}
}

void LevelHandler::LoadFloors()
{
	const uint blacklistCount = 8;
	string blacklist[blacklistCount] =
	{
		"Hubby",
		"tutorial1",
		"tutorial2",
		"tutorial3",
		"tutorial4",
		"tutorial5",
		"tutorial6",
		"tutorial7"
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

void LevelHandler::GenerateTutorial(LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};
	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	string nameStart = "tutorial";
	int first = 1;
	int last = 7;

	FloorPrimer primer =
	{
		{},
		{},
		{}
	};

	primer.networks.Add({});

	float distance = 1.2f/* * 150 * BM_PIXEL_SIZE*/;

	for (int i = first; i <= last; i++)
	{
		primer.rooms.Add({});
		RoomPrimer& r = primer.rooms.Back();

		r.levelIndex = GetLevelByName(nameStart + std::to_string(i));
		r.position = Vec2(0, (i - 1) * distance);
		r.networkIndex = 0;

		if (i == 7)
		{
			r.essenceBloom = true;
		}

		if (i == first)
		{
			r.connections.Add(-1);
			r.connections.Add(i);
		}
		else
		{
			primer.networks[0].tunnels.Add({ (uint)i - 2, (uint)i - 1 });
			r.connections.Add(i - 2);

			if (i == last)
			{
				r.connections.Add(-2);
			}
			else
			{
				r.connections.Add(i);
			}
		}
	}

	Unprime(primer, f, params);

	GenerateNewMinimap(&f);
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

	Unprime(primer, f, eParams);

	// After creating floor structure, create its image.
	GenerateNewMinimap(&f);
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

	float pushOutFactor = 1.3f;
	float radiusFactor = std::sqrtf(2.0f) * pushOutFactor;
	int maxSize = std::max(l.resource->pixelWidth, l.resource->pixelHeight);
	int extraRadius = (int)((radiusFactor - 1.0f) * 0.5f * maxSize);

	float distanceBoundary = maxSize * 0.5f + extraRadius - 3.0f * BM_PIXELS_PER_UNIT;
	distanceBoundary *= distanceBoundary;

	float innerPushOutFactor = 1.1f;
	float innerRadiusFactor = std::sqrtf(2.0f) * innerPushOutFactor;
	int innerMaxSize = std::max(l.resource->pixelWidth, l.resource->pixelHeight);
	float innerDistanceBoundary = (int)((innerRadiusFactor) * 0.5f * innerMaxSize) - 3.0f * BM_PIXELS_PER_UNIT;
	innerDistanceBoundary *= innerDistanceBoundary;

	Vec2 center(l.resource->pixelWidth * 0.5f, l.resource->pixelHeight * 0.5f);

	bool outside = false;

	auto sample = [&](int x, int y)
	{
		outside = false;

		Vec3 pos = l.position + l.rotation * Vec3(x * BM_PIXEL_SIZE - l.resource->worldWidth * 0.5f, -0.2f, -y * BM_PIXEL_SIZE + l.resource->worldHeight * 0.5f);
		bool tunnelEdge = false;

		for (const LevelTunnelRef& r : l.connections)
		{
			Vec3 sideDir = Vec3(r.direction.z, 0.0f, -r.direction.x);
			Vec3 relative = pos - r.position;

			float distance = relative * r.direction;
			float sideDistance = abs(relative * sideDir);

			if (distance > -0.5f)
			{
				if (sideDistance < r.width * 4.0f)
				{
					if (sideDistance < r.width * 0.7f)
					{
						return LevelPixel{ LevelPixelFlagPassable, 0.0f };
					}

					return LevelPixel{ LevelPixelFlagImpassable, 1.0f };
				}
			}
		}

		if ((Vec2(x, y) - center).LengthSq() > innerDistanceBoundary)
		{
			outside = true;
			return LevelPixel{ LevelPixelFlagPassable, 0.0f };
		}

		if (x < 0 || y < 0 || x >= l.resource->pixelWidth || y >= l.resource->pixelHeight)
		{
			return LevelPixel{ LevelPixelFlagImpassable, 0.3f };
		}

		return l.resource->bitmap[x + l.resource->pixelWidth * y];
	};
	
	for (int x = 0 - extraRadius; x < (int)l.resource->pixelWidth + extraRadius; x++)
	{
		for (int y = 0 - extraRadius; y < (int)l.resource->pixelHeight + extraRadius; y++)
		{
			if ((Vec2(x, y) - center).LengthSq() > distanceBoundary)
			{
				continue;
			}

			LevelPixel currentSample = sample(x, y);
			LevelPixelFlag current = currentSample.flags;

			if (outside)
			{
				continue;
			}

			Vec3 newPosition = l.position + l.rotation * Vec3(x * BM_PIXEL_SIZE - l.resource->worldWidth * 0.5f, -0.2f, -y * BM_PIXEL_SIZE + l.resource->worldHeight * 0.5f);

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

			if ((currentSample.flags & LevelPixelFlagTerrainInner & ~LevelPixelFlagTerrainOuter))
			{
				if (noiseVal * currentSample.density < parameters.densityUnwalkableInner)
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
			else if ((current & LevelPixelFlagImpassable))
			{
				Mat4 foliageMatrix =
					Mat::translation3(newPosition) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * 1.5f, 0.05f, 2.0f));

				xP = cs::iclamp(x + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				xM = cs::iclamp(x - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				yP = cs::iclamp(y + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				yM = cs::iclamp(y - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				xPF = sample(xP, y).flags;
				xMF = sample(xM, y).flags;
				yPF = sample(x, yP).flags;
				yMF = sample(x, yM).flags;
				bool edgeTree = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));

				if (noiseVal < parameters.densityUnwalkableOuter * currentSample.density)
				{
					xP = cs::iclamp(x + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
					xM = cs::iclamp(x - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
					yP = cs::iclamp(y + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
					yM = cs::iclamp(y - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
					xPF = sample(xP, y).flags;
					xMF = sample(xM, y).flags;
					yPF = sample(x, yP).flags;
					yMF = sample(x, yM).flags;
					bool edgeTrunk = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));

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
						Mat4 foliageMatrix =
							Mat::translation3(newPosition) *
							Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
							Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * 1.5f, 0.05f, 2.0f));

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
					Mat4 foliageMatrixHuge =
						Mat::translation3(newPosition) *
						Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
						Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * 2.5, 0.1f, 2.2f));

					l.instances[LevelAssetBush1].Add( foliageMatrixHuge );
				}
				else if (noiseVal < parameters.densityWalkable * 0.5f && !edgeStones)
				{
					l.instances[LevelAssetBush1].Add(foliageMatrix);
				}
				else if (noiseVal < parameters.densityWalkable * 0.5f)
				{
					Mat4 foliageMatrix2 =
						Mat::translation3(newPosition) *
						Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
						Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * 1.2f, 0.1, 1.5f));

					l.instances[LevelAssetBush1].Add(foliageMatrix2);
				}
			}
			else if ((sample(x, y).density != 0))
			{
				float density = 1.0f - sample(x, y).density;
				Mat4 foliageMatrix =
					Mat::translation3(newPosition) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3(cs::fclamp(scaleVal * parameters.scaleMultiplierFoliage * (density * (1.0f - parameters.scaleEffectDensity)), 0.1f, 2.0f));

				if (density > parameters.minDensity && noiseVal < parameters.densityWalkable)
				{
					l.instances[LevelAssetBush1].Add(foliageMatrix);
				}
			}
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

void LevelHandler::Unprime(FloorPrimer& primer, LevelFloor& f, EnvironmentalizeParameters parameters)
{
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
					f.startDirection = -exitDirection3;
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

		Environmentalize(l, parameters);
	}
}
