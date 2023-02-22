#include "core.h"
#include "LevelHandler.h"
#include "LevelImporter.h"
#include "Renderer.h"

#include <filesystem>

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

	for (RoomPrimer p : rooms)
	{
		f.rooms.Add({});
		Level& l = f.rooms.Back();

		//l.resource =
		l.position = p.position;
		l.connections.MassAdd(p.connections.Data(), p.connections.Size(), true);
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
