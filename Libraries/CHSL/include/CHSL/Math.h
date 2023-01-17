#pragma once

#include "CHSLTypedef.h"

namespace cs
{
	
	constexpr float c_pi = 3.14159265f;

	int imod(int val, int ceil);
	size_t imod(size_t val, size_t ceil);

	int floor(float val);
	int ceil(float val);

	int imax(int a, int b);
	int imin(int a, int b);
	float fmax(float a, float b);
	float fmin(float a, float b);

	float fclamp(float val, float min, float max);
	int iclamp(int val, int min, int max);

	int iwrap(int val, int min, int max);
	float fwrap(float val, float max);
	float fwrap(float val, float min, float max);

	int iabs(int val);

}