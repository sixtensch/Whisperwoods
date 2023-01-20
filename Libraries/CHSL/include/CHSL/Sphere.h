#pragma once

/* CHSL

	|	Sphere
	|
	|	- Simple sphere implementation. 
	|	- Defined by an origin and a radius.

*/









#include "IRaycastTarget.h"
#include "Vector.h"
#include "Line.h"

namespace cs
{

	class Sphere : public IRaycastTarget
	{
	public:
		Sphere(const Vec3& origin, float radius);
		Sphere(const Sphere&);

		Sphere& operator=(const Sphere&);

		bool operator==(const Sphere&) const;
		bool operator!=(const Sphere&) const;

		const Vec3& GetOrigin() const;
		float GetRadius() const;

		void SetOrigin(const Vec3& origin);
		void SetRadius(float radius);

		bool Contains(const Vec3& point) const;
		bool Intersection(const Line3& line, float& t, bool first = true, bool cullBack = true) const;
		bool Intersection(const Line3& line, Vec3& out, bool first = true, bool cullBack = true) const;

		virtual bool Raycast(const Line3& line, HitInfo& out) const override;

	private:
		Vec3 m_origin;
		float m_radius;
	};

}
