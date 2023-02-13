#include "core.h"
#include "LevelImporter.h"

#include "Resources.h"
#include "RenderCore.h"

void Floodfill(Point2 position, cs::List<Point2>& edges, cs::Color4*& data, int index, int w, int h)
{
	data[position.x + position.y * w] = Point4(0, 255, 0, 255);

	Point2 offsets[] =
	{
		{ 1, 0 }, { 0, 1 },
		{ -1, 0 }, { 0, -1 },
	};

	bool found = false;

	for (int i = 0; i < 4; i++)
	{
		Point2 p = position + offsets[i];

		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
		{
			Point3 c = data[p.x + p.y * w];

			if (c == Point3(255, 255, 255))
			{
				found = true;
			}
			else if (c == Point3(0, 0, 0))
			{
				Floodfill(p, edges, data, index, w, h);
			}
		}
	}

	if (found)
	{

	}
}

bool LevelImporter::ImportImage(string textureName, const RenderCore* core, LevelResource* const outLevel)
{
	outLevel->source = (const TextureResource*)Resources::Get().GetResource(ResourceTypeTexture, textureName);

	if (!outLevel->source)
	{
		LOG_ERROR("Failed to load level image with source [%s], no texture with that name exists.", textureName);
		return false;
	}



	// Dump texture data

	cs::Color4* data;
	uint stride;

	core->DumpTexture(
		outLevel->source->texture2D.Get(),
		&outLevel->pixelWidth,
		&outLevel->pixelHeight,
		&data);

	if (outLevel->pixelWidth < BM_MIN_SIZE || outLevel->pixelHeight < BM_MIN_SIZE)
	{
		LOG_ERROR("Failed to load level image with source [%s], image too small. Size [%i * %i].", textureName, outLevel->pixelWidth, outLevel->pixelHeight);
		return false;
	}

	outLevel->worldWidth = outLevel->pixelWidth * BM_PIXEL_SIZE;
	outLevel->worldHeight = outLevel->pixelHeight * BM_PIXEL_SIZE;
	outLevel->bitmap = unique_ptr<LevelPixel[]>(new LevelPixel[outLevel->pixelWidth * outLevel->pixelHeight]);



	// Parse the map

	LOG("Loading level image with source [%s]. Size [%i * %i].", textureName, outLevel->pixelWidth, outLevel->pixelHeight);

	auto floodfill = [&](Point2 position, int index) 
	{
		floodfill(position, index);
	};

	for (uint x = 0; x < outLevel->pixelWidth; x++)
	{
		for (uint y = 0; y < outLevel->pixelHeight; y++)
		{
			uint i = x + y * outLevel->pixelWidth;
			Point4 c = data[i];

			if (c == Point4(255, 255, 255, 255))
			{
				outLevel->bitmap[i] = Passable;
				continue;
			}

			if (c == Point4(0, 255, 0, 255))
			{
				outLevel->bitmap[i] = TerrainOuter;
				continue;
			}

			if (c == Point4(0, 127, 0, 255))
			{
				outLevel->bitmap[i] = TerrainInner;
				continue;
			}

			if (c.x)
			{
				outLevel->bitmap[i] = TerrainOuter;
				continue;
			}
		}
	}

	delete[] data;
}
