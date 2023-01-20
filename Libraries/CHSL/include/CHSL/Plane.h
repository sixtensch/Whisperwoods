#pragma once

/* CHSL

	|	Plane
	|
	|	- 3D-plane implementation.
	|	- Defined by an origin position, and a plane normal.

*/









#include "IRaycastTarget.h"
#include "Vector.h"
#include "Line.h"

namespace cs
{

	class Plane : public IRaycastTarget
	{
	public:
		Plane();
		Plane(Vec3 origin, Vec3 normal);
		Plane(Vec3 origin, Vec3 v1, Vec3 v2);
		Plane(const Plane&);

		Plane& operator=(const Plane&);

		bool operator==(const Plane&) const;
		bool operator!=(const Plane&) const;

		const Vec3& GetOrigin() const;
		const Vec3& GetNormal() const;

		void SetOrigin(const Vec3& origin);
		void SetNormal(const Vec3& normal);

		bool IsBelow(const Vec3& vector) const;
		bool IsAbove(const Vec3& vector) const;

		bool Equivalent(const Plane& plane) const;
		bool Contains(const Vec3& vector) const;
		bool Contains(const Line3& line) const;
		bool Parallel(const Line3& line) const;
		bool Parallel(const Plane& plane) const;
		bool Perpendicular(const Line3& line) const;

		bool Intersection(const Line3& line, float& t) const;
		bool Intersection(const Line3& line, Vec3& out) const;
		bool Intersection(const Plane& plane, Line3& out) const;

		bool Raycast(const Line3& line, HitInfo& out) const override;

	private:
		void UpdateD();

	private: 
		Vec3 m_origin;
		Vec3 m_normal;
		float m_d;
	};

}

#ifdef CHSL_LINEAR

using cs::Plane;

#endif
