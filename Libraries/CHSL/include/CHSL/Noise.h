#pragma once

#include "Vector.h"

namespace cs
{

	class Noise1D
	{
	public:
		Noise1D() {}
		Noise1D(const Noise1D& copy) {}
		virtual ~Noise1D() {}

		// Result is scaled to the range [0 -> 1]
		virtual float Gen1D(float x) = 0;
	};

	class Noise2D : public Noise1D
	{
	public:
		Noise2D() {}
		Noise2D(const Noise2D& copy) {}
		virtual ~Noise2D() {}

		//virtual float Gen1D(float x) override	= 0;

		// Result is scaled to the range [0 -> 1]
		virtual float Gen2D(float x, float y) = 0;
		// Result is scaled to the range [0 -> 1]
		float Gen2D(cs::Vec2 pos)
		{
			return Gen2D(pos.x, pos.y);
		}
	};

	class Noise3D : public Noise2D
	{
	public:
		Noise3D() {}
		Noise3D(const Noise3D& copy) {}
		virtual ~Noise3D() {}

		//virtual float Gen1D(float x) override			= 0;
		//virtual float Gen2D(float x, float y) override	= 0;

		// Result is scaled to the range [0 -> 1]
		virtual float Gen3D(float x, float y, float z) = 0;
		// Result is scaled to the range [0 -> 1]
		float Gen3D(cs::Vec3 pos)
		{
			return Gen3D(pos.x, pos.y, pos.z);
		};
	};

}

//class NoiseGrid : public Noise3D
//{
//
//};
//
//class NoiseSwishy : public Noise3D
//{
//
//};