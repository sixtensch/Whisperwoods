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
			m_resources.Add({});

			LevelResource* r = &m_resources.Back();
			string s = item.path().filename().string();
			Renderer::LoadLevel(r, s);

			m_resourceIndices[r->exits.Size() - 1].Add(m_resources.Size() - 1);
		}
	}
}

void LevelHandler::GenerateFloor(LevelFloor* outFloor)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);



	// Add a level

	f.rooms.Add({});
	Level& l = f.rooms.Back();

	cs::Random r;
	
	float levelScale = 1.0f;

	l.resource = &m_resources[0];
	Vec3 offset = l.position + Vec3(l.resource->worldWidth * 0.5f, 0, -l.resource->worldHeight * 0.5f);

	for (int x = 0; x < (int)l.resource->pixelWidth; x++)
	{
		for (int y = 0; y < (int)l.resource->pixelHeight; y++)
		{
			if ((l.resource->bitmap[x + l.resource->pixelWidth * y] & ~Passable) && r.Get(10) == 0)
			{
				Mat4 instanceMatrix = Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE) * levelScale) * Mat::rotation3(cs::c_pi * -0.5f, r.Getf(0, cs::c_pi * 2), 0.0f) * Mat::scale3(0.1f);
				l.instances[LevelAssetBush1].Add(instanceMatrix);
			}
			else if ((l.resource->bitmap[x + l.resource->pixelWidth * y] & ~Passable) && r.Get(10) == 0)
			{
				Mat4 instanceMatrix = Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, 0, y * BM_PIXEL_SIZE) * levelScale) * Mat::rotation3(cs::c_pi * -0.5f, r.Getf(0, cs::c_pi * 2), 0.0f) * Mat::scale3(0.1f);
				l.instances[LevelAssetBush2].Add(instanceMatrix);
			}
		}
	}
}
