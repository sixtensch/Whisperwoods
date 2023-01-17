#pragma once

#include "Noise.h"
#include "CHSLTypedef.h"

namespace cs
{

	class NoiseSimplex : public cs::Noise3D
	{
	public:
		static const unsigned int PERM_COUNT = 0x200;

	private:
		byte* permutations;

	public:
		NoiseSimplex();
		NoiseSimplex(uint seed);
		NoiseSimplex(byte* permMatrix, bool copy); // Permutation matrix: 512 (0x200) unsigned chars / bytes
		~NoiseSimplex();

		void PerlinGenMatrix();
		void PerlinGenMatrix(uint seed);
		void PerlinGenMatrix(byte* permMatrix, bool copy);

		float Gen1D(float x) override;
		float Gen2D(float x, float y) override;
		float Gen3D(float x, float y, float z) override;

		float Gradient(byte permHash, float x);
		float Gradient(byte permHash, float x, float y);
		float Gradient(byte permHash, float x, float y, float z);
	};

}
