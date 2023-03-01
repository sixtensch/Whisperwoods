#include "core.h"
#include "LevelHandler.h"
#include "LevelImporter.h"
#include "Renderer.h"

#include <filesystem>

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
			cs::fmin(p1a.x, p1b.x) < x &&
			cs::fmin(p1a.y, p1b.y) < y &&
			cs::fmax(p1a.x, p1b.x) > x &&
			cs::fmax(p1a.y, p1b.y) > y;
	}
}

LevelHandler* LevelHandler::s_handler = nullptr;

LevelHandler::LevelHandler()
{

}

void LevelHandler::LoadFloors()
{
	for (const auto& item : std::filesystem::directory_iterator(DIR_LEVELS))
	{
		if (item.path().extension() == ".png")
		{
			m_resources.Add(make_shared<LevelResource>());

			shared_ptr<LevelResource>& r = m_resources.Back();
			string s = item.path().filename().string();
			Renderer::LoadLevel(r.get(), s);

			m_resourceIndices[r->exits.Size() - 1].Add(m_resources.Size() - 1);
		}
	}
}

void LevelHandler::GenerateFloor(LevelFloor* outFloor, uint seed, uint roomCount, int rep)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

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
	};

	cs::List<RoomPrimer> rooms;



	// Add a level

	cs::Random r(seed);

	// Generate
	for (uint i = 0; i < roomCount; i++)
	{
		rooms.Add({ {}, { r.Getf(-5.0f, 5.0f), r.Getf(-5.0f, 5.0f) }, {} });
	}

	// Push apart
	for (int repetitions = 0; repetitions < rep; repetitions++)
	{
		for (uint i = 0; i < roomCount; i++)
		{
			rooms[i].push = { 0, 0 };

			for (uint j = 0; j < roomCount; j++)
			{
				if (i == j)
				{
					continue;
				}

				Vec2 direction = rooms[j].position - rooms[i].position;
				rooms[i].push += direction.Normalized() / cs::fmax(direction.LengthSq(), 0.3f);
			}
		}

		for (uint i = 0; i < roomCount; i++)
		{
			rooms[i].position += rooms[i].push * -0.5f;
		}
	}

	// Link room primers
	cs::List<TunnelPrimerNetwork> networks;
	cs::List<uint> connectionCounts;
	cs::List<uint> loose;
	cs::List<uint> looseNext;
	for (uint i = 0; i < (uint)rooms.Size(); i++)
	{
		loose.Add(i);
		connectionCounts.Add(0);
	}

	auto pred = [&](uint a, uint b) { return rooms[a].position.x < rooms[b].position.x; };
	auto clear = [&](uint start, uint end)
	{
		if (connectionCounts[start] > 2 || connectionCounts[end] > 2)
		{
			return false;
		}

		Vec2 ori = rooms[start].position;
		Vec2 dir = rooms[end].position - ori;
		dir.Normalize();

		for (const TunnelPrimerNetwork& network : networks)
		{
			for (const TunnelPrimer& tunnel : network.tunnels)
			{
				if (start == tunnel.start || start == tunnel.end || end == tunnel.start || end == tunnel.end)
				{
					continue;
				}

				Vec2 otherOri = rooms[tunnel.start].position;
				Vec2 otherDir = rooms[tunnel.end].position - otherOri;
				float otherLen = otherDir.Length();
				otherDir.Normalize();

				float t = (otherDir.y * (otherOri.x - ori.x) + ori.y - otherOri.y) / (dir.x * otherDir.y - dir.y);

				if (t > 0.01f && t < otherLen - 0.01f)
				{
					return false;
				}
			}
		}

		return true;
	};

	//std::sort(loose.Data(), loose.Data() + loose.Size(), pred);

	int iterations = 0;

	do
	{
		while (loose.Size() > 0)
		{
			uint currentIndex = loose.Back();
			Vec2 current = rooms[currentIndex].position;

			if (currentIndex == 4)
			{
				int a = 0;
			}

			float previousClosest = 0.0f;

			for (uint attempt = 0; attempt < 3; attempt++)
			{
				int closest = 0;
				float closestDistance = INFINITY;

				for (int i = rooms.Size() - 1; i >= 0; i--)
				{
					if ((uint)i == currentIndex)
					{
						continue;
					}

					Vec2 target = rooms[i].position;
					float distance = (current - target).LengthSq();
					if (distance < closestDistance && distance > previousClosest + 0.0001f)
					{
						closest = i;
						closestDistance = distance;
					}
				}

				if (clear(currentIndex, closest))
				{
					if (rooms[closest].networkIndex >= 0)
					{
						networks[rooms[closest].networkIndex].tunnels.Add({ (uint)closest, currentIndex });
						rooms[currentIndex].networkIndex = rooms[closest].networkIndex;
					}
					else
					{
						networks.Add({ { { currentIndex, (uint)closest } } });
						rooms[currentIndex].networkIndex = networks.Size() - 1;
						rooms[closest].networkIndex = networks.Size() - 1;

						for (int i = 0; i < loose.Size(); i++)
						{
							if (loose[i] == closest)
							{
								loose.Remove(i);
								break;
							}
						}
					}

					connectionCounts[currentIndex]++;
					connectionCounts[closest]++;

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
	while (loose.Size() > 0 && iterations < 5);

	for (const TunnelPrimerNetwork& network : networks)
	{
		for (const TunnelPrimer& tunnel : network.tunnels)
		{
			rooms[tunnel.start].connections.Add(tunnel.end);
			rooms[tunnel.end].connections.Add(tunnel.start);

			f.tunnels.Add({ tunnel.start, tunnel.end });
		}
	}



	// Create level objects

	for (RoomPrimer p : rooms)
	{
		f.rooms.Add({});
		Level& l = f.rooms.Back();

		//l.resource =
		l.position = p.position;
		l.connections.MassAdd(p.connections);
	}
}

void LevelHandler::GenerateTestFloor(LevelFloor* outFloor)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	cs::Random r;



	// Add a level

	AddLevelName(f, "TestBig");
	Environmentalize(f.rooms.Back(), r);
}

void LevelHandler::Environmentalize(Level& l, cs::Random& r)
{
	Vec3 offset = l.position + Vec3(l.resource->worldWidth * 0.5f, 0, -l.resource->worldHeight * 0.5f);

	for (int x = 0; x < (int)l.resource->pixelWidth; x++)
	{
		for (int y = 0; y < (int)l.resource->pixelHeight; y++)
		{
			if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagImpassable) && r.Get(10) == 0)
			{
				Mat4 instanceMatrix = Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE)) * Mat::rotation3(cs::c_pi * -0.5f, r.Getf(0, cs::c_pi * 2), 0.0f) * Mat::scale3(0.1f);
				l.instances[LevelAssetBush1].Add(instanceMatrix);
			}
			else if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagImpassable) && r.Get(10) == 0)
			{
				Mat4 instanceMatrix = Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE)) * Mat::rotation3(cs::c_pi * -0.5f, r.Getf(0, cs::c_pi * 2), 0.0f) * Mat::scale3(0.1f);
				l.instances[LevelAssetBush2].Add(instanceMatrix);
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
