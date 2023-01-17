#pragma once

#include "IRaycastTarget.h"

namespace cs
{

	class Triangle : public IRaycastTarget
	{
	public:
		Triangle(Vec3 v1, Vec3 v2, Vec3 v3);

		bool Raycast(const Line3& line, HitInfo& out) const;

	private:
		Vec3 m_vertices[3];
	};

}