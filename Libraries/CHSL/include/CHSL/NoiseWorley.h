#pragma once

/* CHSL

	|	NoiseWorley
	|
	|	- Worley noise implementation for 1, 2, and 3 dimensions.
	|	- Akin to a voronoi algorithm.

*/









#include "Noise.h"
#include "GeneralEnums.h"

namespace cs
{

	class NoiseWorley : public cs::Noise3
	{
	public:
		enum class WorleyType
		{
			Nearest,
			SecondNearest,
			CompoundNearest
		};

	private:
		const int GRID_CLUSTER_RADIUS = 3;

		unsigned int xs, ys, zs, s, cellCount, savedPositionsCount;
		Dimension d;
		cs::Point3 currentCenter;
		cs::Vec3* cellLocalPositions;
		cs::Vec3* savedPositions;

	public:
		NoiseWorley(unsigned int seed, unsigned int xSize);
		NoiseWorley(unsigned int seed, unsigned int xSize, unsigned int ySize);
		NoiseWorley(unsigned int seed, unsigned int xSize, unsigned int ySize, unsigned int zSize);
		~NoiseWorley();

		float Gen1D(float x) override;
		float Gen2D(float x, float y) override;
		float Gen3D(float x, float y, float z) override;

	private:
		float GetClosestCurrentDistance(int count, float x, float y, float z);
	};

}
