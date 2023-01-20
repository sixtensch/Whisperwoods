#pragma once

/* CHSL

	|	IRaycastTarget
	|
	|	- Interface for types with defined line/ray intersection.

*/









#include "Line.h"

namespace cs
{

	struct HitInfo
	{
		float t;

		Vec3 normal;

		float u;
		float v;
	};

	class IRaycastTarget
	{
	public:
		virtual bool Raycast(const Line3& line, HitInfo& out) const = 0;
	};

}