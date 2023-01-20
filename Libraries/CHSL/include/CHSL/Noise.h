#pragma once

/* CHSL

	|	Noise1, Noise2, Noise3
	|
	|	- Abstract base classes for implementing noise algorithms in 1, 2, and 3 dimensions.

*/









#include "Vector.h"

namespace cs
{

	class Noise1
	{
	public:
		Noise1() {}
		Noise1(const Noise1& copy) {}
		virtual ~Noise1() {}

		// Result is scaled to the range [0 -> 1]
		virtual float Gen1D(float x) = 0;
	};

	class Noise2 : public Noise1
	{
	public:
		Noise2() {}
		Noise2(const Noise2& copy) {}
		virtual ~Noise2() {}

		// Result is scaled to the range [0 -> 1]
		virtual float Gen2D(float x, float y) = 0;
		// Result is scaled to the range [0 -> 1]
		float Gen2D(cs::Vec2 pos)
		{
			return Gen2D(pos.x, pos.y);
		}
	};

	class Noise3 : public Noise2
	{
	public:
		Noise3() {}
		Noise3(const Noise3& copy) {}
		virtual ~Noise3() {}


		// Result is scaled to the range [0 -> 1]
		virtual float Gen3D(float x, float y, float z) = 0;
		// Result is scaled to the range [0 -> 1]
		float Gen3D(cs::Vec3 pos)
		{
			return Gen3D(pos.x, pos.y, pos.z);
		};
	};

}
