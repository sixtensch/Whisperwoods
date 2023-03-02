#include "core.h"
#include "LevelImporter.h"

#include "Resources.h"
#include "RenderCore.h"

struct PatrolPrimer
{
	struct Point
	{
		Vec2 position;
		uint index = 1000;
	};

	uint enemyIndex;
	cs::List<Point> points;
};

bool PrimerPredicate(const PatrolPrimer& a, const PatrolPrimer& b)
{
	return a.enemyIndex < b.enemyIndex;
}

bool PrimerPointPredicate(const PatrolPrimer::Point& a, const PatrolPrimer::Point& b)
{
	return a.index < b.index;
}

void Floodfill(Point2 position, cs::List<Point2>& edges, cs::List<Point2>& directions, cs::Color4*& data, int w, int h)
{
	data[position.x + position.y * w] = cs::Color4(0, 63, 0, 255);

	Point2 offsets[] =
	{
		{ 1, 0 }, { 0, 1 },
		{ -1, 0 }, { 0, -1 },
	};

	bool found = false;
	Point2 direction(0, 0);

	for (int i = 0; i < 4; i++)
	{
		Point2 p = position + offsets[i];

		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
		{
			cs::Color4 c4 = data[p.x + p.y * w];
			Point3 c(c4.r, c4.g, c4.b);

			if (c == Point3(255, 255, 255) || c.y > 200)
			{
				found = true;
				direction -= offsets[i];
			}
			else if (c == Point3(0, 0, 0))
			{
				Floodfill(p, edges, directions, data, w, h);
			}
		}
	}

	if (found)
	{
		edges.Add(position);
		directions.Add(direction);
	}
}

bool LevelImporter::ImportImage(string textureName, const RenderCore* core, LevelResource* const outLevel)
{
	outLevel->source = (const TextureResource*)Resources::Get().GetResource(ResourceTypeTexture, textureName);
	outLevel->name = textureName.substr(0, textureName.size() - 4);

	if (!outLevel->source)
	{
		LOG_ERROR("Failed to load level image with source [%s], no texture with that name exists.", textureName.c_str());
		return false;
	}



	// Dump texture data

	cs::Color4* data;

	core->DumpTexture(
		outLevel->source->texture2D.Get(),
		&outLevel->pixelWidth,
		&outLevel->pixelHeight,
		&data);

	if (outLevel->pixelWidth < BM_MIN_SIZE || outLevel->pixelHeight < BM_MIN_SIZE)
	{
		LOG_ERROR("Failed to load level image with source [%s], image too small. Size [%i * %i].", textureName.c_str(), outLevel->pixelWidth, outLevel->pixelHeight);
		return false;
	}

	outLevel->worldWidth = outLevel->pixelWidth * BM_PIXEL_SIZE;
	outLevel->worldHeight = outLevel->pixelHeight * BM_PIXEL_SIZE;
	outLevel->bitmap = shared_ptr<LevelPixel[]>(new LevelPixel[outLevel->pixelWidth * outLevel->pixelHeight]);



	// Parse the map

	LOG("Loading level image with source [%s]. Size [%i * %i].", textureName.c_str(), outLevel->pixelWidth, outLevel->pixelHeight);

	cs::List<Point2> edges;
	cs::List<Point2> directions;

	cs::List<PatrolPrimer> openPrimers;
	cs::List<PatrolPrimer> closedPrimers;

	for (uint x = 0; x < outLevel->pixelWidth; x++)
	{
		for (uint y = 0; y < outLevel->pixelHeight; y++)
		{
			uint i = x + y * outLevel->pixelWidth;
			Point2 p(x, y);
			Point3 c = Point3(data[i].r, data[i].g, data[i].b);

			if (c == Point3(255, 255, 255))
			{
				outLevel->bitmap[i] = { LevelPixelFlagPassable, 0.0f };
				continue;
			}

			if (c == Point3(0, 63, 0))
			{
				outLevel->bitmap[i] = { LevelPixelFlagTerrainOuter, 1.0f };
				continue;
			}

			if (c == Point3(0, 127, 0))
			{
				outLevel->bitmap[i] = { LevelPixelFlagTerrainInner, 1.0f };
				continue;
			}

			// Open patroll route
			if (c.x == 255 && c.y < 63)
			{
				outLevel->bitmap[i] = { LevelPixelFlagPassable, 0.0f };
				bool found = false;

				for (int i = 0; i < openPrimers.Size() && !found; i++)
				{
					if (openPrimers[i].enemyIndex == c.z)
					{
						openPrimers[i].points.Add({ (Vec2)p, (uint)c.y });
						found = true;
					}
				}

				if (!found)
				{
					openPrimers.Add({ (uint)c.z, { { (Vec2)p, (uint)c.y } } });
				}

				continue;
			}

			// Closed patroll route
			if (c.z == 255 && c.y < 63)
			{
				outLevel->bitmap[i] = { LevelPixelFlagPassable, 0.0f };
				bool found = false;

				for (int i = 0; i < closedPrimers.Size() && !found; i++)
				{
					if (closedPrimers[i].enemyIndex == c.y)
					{
						closedPrimers[i].points.Add({ (Vec2)p, (uint)c.x });
						found = true;
					}
				}

				if (!found)
				{
					closedPrimers.Add({ (uint)c.y, { { (Vec2)p, (uint)c.x } } });
				}

				continue;
			}

			if (c.y > 200)
			{
				outLevel->bitmap[i] = { LevelPixelFlagPassable, (c.y - 200) / 55.0f };
				continue;
			}

			if (c == Point3(0, 0, 0))
			{
				edges.Clear(false);
				directions.Clear(false);

				Floodfill(p, edges, directions, data, outLevel->pixelWidth, outLevel->pixelHeight);

				Point2 edgeA = edges[0];
				Point2 edgeB = edges[0];
				float edgeDistanceSq = 0.0f;
				Point2 direction;

				for (int i = 0; i < edges.Size(); i++)
				{
					float dsqA = (edges[i] - edgeA).LengthSq();
					float dsqB = (edges[i] - edgeB).LengthSq();
					if (dsqA >= dsqB && dsqA > edgeDistanceSq)
					{
						edgeB = edges[i];
						edgeDistanceSq = dsqA;
					}

					if (dsqB > dsqA && dsqB > edgeDistanceSq)
					{
						edgeA = edges[i];
						edgeDistanceSq = dsqB;
					}

					direction += directions[i];
				}

				Vec2 nDirection = (Vec2)direction;
				Vec2 edgeDiff = (Vec2)edgeB - (Vec2)edgeA;
				Vec2 edgeDir(edgeDiff.y, -edgeDiff.x);
				edgeDir.Normalize();

				Vec2 trueDirection = (edgeDir * nDirection > 0) ? edgeDir : -edgeDir;

				outLevel->exits.Add(
					{
						(Vec2)edgeA + edgeDiff * 0.5f,
						trueDirection,
						cs::fclamp(std::sqrtf(edgeDistanceSq), 6.0f, 10.0f),
						cs::fwrap(std::atan2f(trueDirection.y, trueDirection.x), 0.0f, 2 * cs::c_pi)
					});

				continue;
			}
		}
	}

	std::sort(
		&outLevel->exits.Front(), 
		&outLevel->exits.Back() + 1,
		[](const LevelExit& a, const LevelExit& b) { return a.angle < b.angle; });
	
	if (openPrimers.Size() > 0)
	{
		std::sort(&openPrimers.Front(), &openPrimers.Back() + 1, PrimerPredicate);

		for (PatrolPrimer& p : openPrimers)
		{
			std::sort(&p.points.Front(), &p.points.Back() + 1, PrimerPointPredicate);

			outLevel->patrolsOpen.Add({ p.enemyIndex, {} });
			for (PatrolPrimer::Point pt : p.points)
			{
				outLevel->patrolsOpen.Back().controlPoints.Add((Vec2)pt.position);
			}
		}
	}

	if (closedPrimers.Size() > 0)
	{
		std::sort(&closedPrimers.Front(), &closedPrimers.Back() + 1, PrimerPredicate);

		for (PatrolPrimer& p : closedPrimers)
		{
			std::sort(&p.points.Front(), &p.points.Back() + 1, PrimerPointPredicate);

			outLevel->patrolsClosed.Add({ p.enemyIndex, {} });
			for (PatrolPrimer::Point pt : p.points)
			{
				outLevel->patrolsClosed.Back().controlPoints.Add((Vec2)pt.position);
			}
		}
	}

	delete[] data;

	return true;
}
