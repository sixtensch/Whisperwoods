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

void LevelHandler::GenerateFloor(LevelFloor* outFloor, uint seed, uint roomCount, uint pushSteps)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	FloorPrimer primer =
	{
		{ seed },
		{},
		{}
	};

	

	// Bogo-ing

	bool success = false;
	uint attempts = 0;

	//primer.rooms.Add(
	//	{
	//		{ 1, 2 },
	//		{ 0.0f, 0.0f },
	//		{},
	//		0,
	//		GetLevelByName("Test2-3")
	//	}
	//);

	//primer.rooms.Add(
	//	{
	//		{ 0 },
	//		{ 10.0f, 0.0f },
	//		{},
	//		0,
	//		GetLevelByName("Test1-1")
	//	}
	//);

	//primer.rooms.Add(
	//	{
	//		{ 0 },
	//		{ 0.0f, 10.0f },
	//		{},
	//		0,
	//		GetLevelByName("Test1-2")
	//	}
	//);

	//AngleRooms(primer);
	//EvaluateRoom(primer.rooms[0]);
	//EvaluateRoom(primer.rooms[1]);
	//EvaluateRoom(primer.rooms[2]);



	float degToRad = cs::c_pi / 180.0f;
	float dotThreshold = std::cosf(5.0f * degToRad);
	
	do
	{
		attempts++;
		CreateNodes(primer, roomCount, pushSteps);
		success = TryConnecting(primer, dotThreshold);

		if (!success)
		{
			continue;
		}

		success = TryLeveling(primer, true, 3);
	} 
	while (!success);

	LOG("Generated map network. Attempts: %i", (int)attempts);



	// Create level objects

	for (const RoomPrimer& p : primer.rooms)
	{
		f.rooms.Add({});
		Level& l = f.rooms.Back();

		//l.resource =
		l.position = p.position;
		l.rotation = Quaternion::GetEuler(0.0f, p.angleOffset, 0.0f);
		l.resource = m_resources[p.levelIndex].get();

		for (uint i = 0; i < (uint)p.connections.Size(); i++)
		{
			l.connections.Add(p.connections[(i - p.connectionOffset) % p.connections.Size()]);
		}
	}

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
	unique_ptr<cs::List<uint>[]> levelRefs(new cs::List<uint>[m_resources.Size()]);

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

void LevelHandler::AngleRooms(FloorPrimer& f)
{
	for (RoomPrimer& r : f.rooms)
	{
		r.angles.Clear(false);
		for (uint i = 0u; i < (uint)r.connections.Size(); i++)
		{
			Vec2 direction = f.rooms[r.connections[i]].position - r.position;
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

		/*if (r.levelIndex == GetLevelByName("Test3-1"))
		{
			int a = 0;
		}*/

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
				deviationScore += std::fabs(offsetDeviations.Back());
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
			deviationScore += std::fabs(angles[j] - average);
		}

		if (deviationScore < result)
		{
			result = deviationScore;
		}
	}

	return result;
}
