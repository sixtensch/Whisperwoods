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

void LevelHandler::GenerateHubby( LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};
	f.startRoom = 0;
	f.startPosition = Vec3( 0, 0, 0 );


	// Add a level
	AddLevelName( f, "Hubby" );
	Environmentalize( f.rooms.Back(), params);
}

void LevelHandler::GenerateFloor(LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};
	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	// Add a level
	f.rooms.Add({});
	Level& l = f.rooms.Back();


	
	l.resource = m_resources[0].get();
	Environmentalize(l, params);
}

void LevelHandler::GenerateTestFloor(LevelFloor* outFloor, EnvironmentalizeParameters params)
{
	LevelFloor& f = *outFloor;
	f = LevelFloor{};

	f.startRoom = 0;
	f.startPosition = Vec3(0, 0, 0);

	// Add a level

	AddLevelName(f, "sixthLevel");
	Environmentalize(f.rooms.Back(), params);
}

void LevelHandler::Environmentalize(Level& l, EnvironmentalizeParameters parameters)
{
	Vec3 offset = l.position + Vec3(l.resource->worldWidth * 0.5f, 0, -l.resource->worldHeight * 0.5f);
	cs::NoiseSimplex simplexerSpawn(parameters.spawnSeed);
	cs::NoiseSimplex simplexerScale(parameters.scaleSeed);
	cs::NoiseSimplex simplexerRotate(parameters.rotationSeed);
	cs::NoiseSimplex simplexerDiversity(parameters.diversitySeed);
	
	for (int x = 0; x < (int)l.resource->pixelWidth; x++)
	{
		for (int y = 0; y < (int)l.resource->pixelHeight; y++)
		{
			LevelPixelFlag current = l.resource->bitmap[x + l.resource->pixelWidth * y].flags;
			int xP = cs::iclamp(x + parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelWidth - 1);
			int xM = cs::iclamp(x - parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelWidth - 1);
			int yP = cs::iclamp(y + parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelHeight - 1);
			int yM = cs::iclamp(y - parameters.edgeSampleDistanceStones, 0, (int)l.resource->pixelHeight - 1);
			LevelPixelFlag xPF = l.resource->bitmap[xP + l.resource->pixelWidth * y].flags;
			LevelPixelFlag xMF = l.resource->bitmap[xM + l.resource->pixelWidth * y].flags;
			LevelPixelFlag yPF = l.resource->bitmap[x + l.resource->pixelWidth * yP].flags;
			LevelPixelFlag yMF = l.resource->bitmap[x + l.resource->pixelWidth * yM].flags;
			bool edgeStones = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));


			float noiseVal = simplexerSpawn.Gen2D(x*parameters.xMult, y * parameters.yMult);
			float rotateVal = (cs::c_pi * 2) * simplexerRotate.Gen2D(x * parameters.xMult, y * parameters.yMult)*parameters.rotateMult;
			float scaleVal = parameters.scaleBase + simplexerScale.Gen2D(x * parameters.xMult, y * parameters.yMult) * parameters.scaleMult;
			float diversityVal = simplexerDiversity.Gen2D(x * parameters.xMult, y * parameters.yMult);

			Mat4 stoneMatrix =
				Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE)) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3(scaleVal * parameters.scaleMultiplierStones);

			Mat4 treeMatrix =
				Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE)) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3(scaleVal * parameters.scaleMultiplierTrees);

			Mat4 trunkMatrix =
				Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE)) *
				Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
				Mat::scale3(scaleVal * parameters.scaleMultiplierTrees*0.25f);

			if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagTerrainInner & ~LevelPixelFlagTerrainOuter))
			{

				xP = cs::iclamp(x + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				xM = cs::iclamp(x - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
				yP = cs::iclamp(y + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				yM = cs::iclamp(y - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
				xPF = l.resource->bitmap[xP + l.resource->pixelWidth * y].flags;
				xMF = l.resource->bitmap[xM + l.resource->pixelWidth * y].flags;
				yPF = l.resource->bitmap[x + l.resource->pixelWidth * yP].flags;
				yMF = l.resource->bitmap[x + l.resource->pixelWidth * yM].flags;
				bool edgeTree = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));


				Mat4 foliageMatrix =
					Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE)) *
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
			else if ((l.resource->bitmap[x + l.resource->pixelWidth * y].flags & LevelPixelFlagImpassable))
			{
				if (noiseVal < parameters.densityUnwalkableOuter)
				{
					xP = cs::iclamp(x + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
					xM = cs::iclamp(x - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelWidth - 1);
					yP = cs::iclamp(y + parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
					yM = cs::iclamp(y - parameters.edgeSampleDistanceTrunks, 0, (int)l.resource->pixelHeight - 1);
					xPF = l.resource->bitmap[xP + l.resource->pixelWidth * y].flags;
					xMF = l.resource->bitmap[xM + l.resource->pixelWidth * y].flags;
					yPF = l.resource->bitmap[x + l.resource->pixelWidth * yP].flags;
					yMF = l.resource->bitmap[x + l.resource->pixelWidth * yM].flags;
					bool edgeTrunk = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));

					xP = cs::iclamp(x + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
					xM = cs::iclamp(x - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelWidth - 1);
					yP = cs::iclamp(y + parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
					yM = cs::iclamp(y - parameters.edgeSampleDistanceTrees, 0, (int)l.resource->pixelHeight - 1);
					xPF = l.resource->bitmap[xP + l.resource->pixelWidth * y].flags;
					xMF = l.resource->bitmap[xM + l.resource->pixelWidth * y].flags;
					yPF = l.resource->bitmap[x + l.resource->pixelWidth * yP].flags;
					yMF = l.resource->bitmap[x + l.resource->pixelWidth * yM].flags;
					bool edgeTree = !((current == xPF) && (current == xMF) && (current == yPF) && (current == yPF));

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
					else if (diversityVal < 0.6f && !edgeTree)
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
			}
			else if ((l.resource->bitmap[x + l.resource->pixelWidth * y].density != 0))
			{
				float density = 1.0f - l.resource->bitmap[x + l.resource->pixelWidth * y].density;
				Mat4 foliageMatrix =
					Mat::translation3(offset + Vec3(-x * BM_PIXEL_SIZE, -0.2, y * BM_PIXEL_SIZE)) *
					Mat::rotation3(cs::c_pi * -0.5f, rotateVal, 0.0f) *
					Mat::scale3(scaleVal * parameters.scaleMultiplierFoliage * (density * (1.0f - parameters.scaleEffectDensity)) );

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
