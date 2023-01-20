#pragma once

/* CHSL

	|	BoxUnaligned
	|
	|	-	Unaligned box. 
	|	-	Defined by a centerpoint and directional vectors.

	|	_Box2<T>		( Box2, Box2d, Box2i, Box2u )
	|	_Box3<T>		( Box3, Box3d, Box3i, Box3u )
	|
	|	-	Axis-aligned box.
	|	-	Defined by a corner (position), along with an offset
	|		(size) to a second corner.

*/







#include "IRaycastTarget.h"
#include "Vector.h"

namespace cs
{

	template<typename T>
	class _Box2
	{
	public:
		_Box2();
		_Box2(cs::_Vec2<T> position, cs::_Vec2<T> size);
		_Box2(T x, T y, T width, T height);

		template<typename T2>
		constexpr operator _Box2<T2>();

		// Intersects or touches other rectangle (can share an edge)
		constexpr bool Touches(const _Box2& other) const;
		// Strictly intersects other rectangle (cannot merely share an edge)
		constexpr bool Overlaps(const _Box2& other) const;
		// Wholly contains other rectangle (can share edges)
		constexpr bool Contains(const _Box2& other) const;
		// Strictly contains other rectangle (larger on all sides)
		constexpr bool Envelops(const _Box2& other) const;

		// Contains vector/point (can be on edge)
		constexpr bool Contains(const _Vec2<T>& vector) const;
		// Wholly contains vector/point (cannot be on edge)
		constexpr bool Envelops(const _Vec2<T>& vector) const;
		// Same as Contains
		constexpr bool Touches(const _Vec2<T>& vector) const;
		// Same as Envelops
		constexpr bool Overlaps(const _Vec2<T>& vector) const;

		_Box2& operator=(const _Box2& other);
		constexpr bool operator==(const _Box2& other);
		constexpr bool operator!=(const _Box2& other);

	public:
		cs::_Vec2<T> position;
		cs::_Vec2<T> size;
	};

	template<typename T>
	struct _Box3
	{
	public:
		_Box3();
		_Box3(cs::_Vec3<T> position, cs::_Vec3<T> size);
		_Box3(T x, T y, T z, T width, T height, T depth);
		_Box3(const _Box3&);

		// Intersects or touches other box (can share an edge)
		constexpr bool Touches(const _Box3& other) const;
		// Strictly intersects other box (cannot merely share an edge)
		constexpr bool Overlaps(const _Box3& other) const;
		// Wholly contains other box (can share edges)
		constexpr bool Contains(const _Box3& other) const;
		// Strictly contains other box (larger on all sides)
		constexpr bool Envelops(const _Box3& other) const;

		// Contains vector/point (can be on edge)
		constexpr bool Contains(const _Vec3<T>& vector) const;
		// Wholly contains vector/point (cannot be on edge)
		constexpr bool Envelops(const _Vec3<T>& vector) const;
		// Same as Contains
		constexpr bool Touches(const _Vec3<T>& vector) const;
		// Same as Envelops
		constexpr bool Overlaps(const _Vec3<T>& vector) const;

		_Box3& operator=(const _Box3& other);
		bool operator==(const _Box3& other);
		bool operator!=(const _Box3& other);

	public:
		cs::_Vec3<T> position;
		cs::_Vec3<T> size;
	};

	class BoxUnaligned : public IRaycastTarget
	{
	public:
		BoxUnaligned(Vec3 center, Vec3 xV, Vec3 yV, Vec3 zV);
		BoxUnaligned(Vec3 center, float width, float height, float depth);
		BoxUnaligned(Vec3 center, float width, float height, float depth, float rX, float rY, float rZ);

		bool Raycast(const Line3& line, HitInfo& out) const override;
		bool Intersection(const Line3& line, Vec3& out) const;

	private:
		Vec3 m_origin;
		Vec3 m_x;
		Vec3 m_y;
		Vec3 m_z;
		float m_w;
		float m_h;
		float m_d;
	};



	// Implementation

	template<typename T>
	inline _Box2<T>::_Box2()
		:
		position(0, 0),
		size(0, 0)
	{
	}

	template<typename T>
	inline _Box2<T>::_Box2(cs::_Vec2<T> position, cs::_Vec2<T> size)
		:
		position(position),
		size(size)
	{
	}

	template<typename T>
	inline _Box2<T>::_Box2(T x, T y, T width, T height)
		:
		position(x, y),
		size(width, height)
	{
	}

	template<typename T>
	constexpr inline bool _Box2<T>::Touches(const _Box2& other) const
	{
		return !(
			other.position.x + other.size.x < position.x ||
			other.position.y + other.size.y < position.y ||
			other.position.x > position.x + size.x ||
			other.position.y > position.y + size.y
			);
	}

	template<typename T>
	constexpr inline bool _Box2<T>::Overlaps(const _Box2& other) const
	{
		return !(
			other.position.x + other.size.x <= position.x ||
			other.position.y + other.size.y <= position.y ||
			other.position.x >= position.x + size.x ||
			other.position.y >= position.y + size.y
			);
	}

	template<typename T>
	constexpr inline bool _Box2<T>::Contains(const _Box2& other) const
	{
		return !(
			other.position.x < position.x ||
			other.position.y < position.y ||
			other.position.x + other.size.x > position.x + size.x ||
			other.position.y + other.size.y > position.y + size.y
			);
	}

	template<typename T>
	constexpr inline bool _Box2<T>::Contains(const _Vec2<T>& vector) const
	{
		return !(
			vector.x < position.x ||
			vector.y < position.y ||
			vector.x > position.x + size.x ||
			vector.y > position.y + size.y
			);
	}

	template<typename T>
	inline constexpr bool _Box2<T>::Envelops(const _Box2& other) const
	{
		return !(
			other.position.x <= position.x ||
			other.position.y <= position.y ||
			other.position.x + other.size.x >= position.x + size.x ||
			other.position.y + other.size.y >= position.y + size.y
			);
	}

	template<typename T>
	inline constexpr bool _Box2<T>::Envelops(const _Vec2<T>& vector) const
	{
		return !(
			vector.x <= position.x ||
			vector.y <= position.y ||
			vector.x >= position.x + size.x ||
			vector.y >= position.y + size.y
			);
	}

	template<typename T>
	inline constexpr bool _Box2<T>::Touches(const _Vec2<T>& vector) const
	{
		return !(
			vector.x < position.x ||
			vector.y < position.y ||
			vector.x > position.x + size.x ||
			vector.y > position.y + size.y
			);
	}

	template<typename T>
	inline constexpr bool _Box2<T>::Overlaps(const _Vec2<T>& vector) const
	{
		return !(
			vector.x <= position.x ||
			vector.y <= position.y ||
			vector.x >= position.x + size.x ||
			vector.y >= position.y + size.y
			);
	}

	template<typename T>
	inline _Box2<T>& _Box2<T>::operator=(const _Box2& other)
	{
		position = other.position;
		size = other.size;

		return *this;
	}

	template<typename T>
	constexpr inline bool _Box2<T>::operator==(const _Box2& other)
	{
		return position == other.position && size == other.size;
	}

	template<typename T>
	constexpr inline bool _Box2<T>::operator!=(const _Box2& other)
	{
		return position != other.position || size != other.size;
	}

	template<typename T>
	template<typename T2>
	constexpr inline _Box2<T>::operator _Box2<T2>()
	{
		return _Box2<T2>((T2)position.x, (T2)position.y, (T2)size.x, (T2)size.y);
	}



	// Implementation

	template<typename T>
	inline _Box3<T>::_Box3()
		:
		position(),
		size()
	{
	}

	template<typename T>
	inline _Box3<T>::_Box3(cs::_Vec3<T> position, cs::_Vec3<T> size)
		:
		position(position),
		size(size)
	{
	}

	template<typename T>
	inline _Box3<T>::_Box3(T x, T y, T z, T width, T height, T depth)
		:
		position(x, y, z),
		size(width, height, depth)
	{
	}

	template<typename T>
	inline _Box3<T>::_Box3(const _Box3& lVal)
		:
		position(lVal.position),
		size(lVal.size)
	{
	}


	template<typename T>
	constexpr inline bool _Box3<T>::Touches(const _Box3& other) const
	{
		return !(
			other.position.x + other.size.x < position.x ||
			other.position.y + other.size.y < position.y ||
			other.position.z + other.size.z < position.z ||
			other.position.x > position.x + size.x ||
			other.position.y > position.y + size.y ||
			other.position.z > position.z + size.z 
		);
	}

	template<typename T>
	constexpr inline bool _Box3<T>::Overlaps(const _Box3& other) const
	{
		return !(
			other.position.x + other.size.x <= position.x ||
			other.position.y + other.size.y <= position.y ||
			other.position.z + other.size.z <= position.z ||
			other.position.x >= position.x + size.x ||
			other.position.y >= position.y + size.y ||
			other.position.z >= position.z + size.z
		);
	}

	template<typename T>
	constexpr inline bool _Box3<T>::Contains(const _Box3& other) const
	{
		return !(
			other.position.x < position.x ||
			other.position.y < position.y ||
			other.position.z < position.z ||
			other.position.x + other.size.x > position.x + size.x ||
			other.position.y + other.size.y > position.y + size.y ||
			other.position.z + other.size.z > position.z + size.z
		);
	}

	template<typename T>
	inline constexpr bool _Box3<T>::Envelops(const _Box3& other) const
	{
		return !(
			other.position.x <= position.x ||
			other.position.y <= position.y ||
			other.position.z <= position.z ||
			other.position.x + other.size.x >= position.x + size.x ||
			other.position.y + other.size.y >= position.y + size.y ||
			other.position.z + other.size.z >= position.z + size.z
		);
	}

	template<typename T>
	constexpr inline bool _Box3<T>::Contains(const _Vec3<T>& vector) const
	{
		return !(
			vector.x < position.x ||
			vector.y < position.y ||
			vector.z < position.z ||
			vector.x > position.x + size.x ||
			vector.y > position.y + size.y ||
			vector.z > position.z + size.z
		);
	}

	template<typename T>
	inline constexpr bool _Box3<T>::Envelops(const _Vec3<T>& vector) const
	{
		return !(
			vector.x <= position.x ||
			vector.y <= position.y ||
			vector.z <= position.z ||
			vector.x >= position.x + size.x ||
			vector.y >= position.y + size.y ||
			vector.z >= position.z + size.z
			);
	}

	template<typename T>
	inline constexpr bool _Box3<T>::Touches(const _Vec3<T>& vector) const
	{
		return Contains(vector);
	}

	template<typename T>
	inline constexpr bool _Box3<T>::Overlaps(const _Vec3<T>& vector) const
	{
		return Overlaps(vector);
	}

	template<typename T>
	inline _Box3<T>& _Box3<T>::operator=(const _Box3& other)
	{
		position = other.position;
		size = other.size;
		return *this;
	}

	template<typename T>
	inline bool _Box3<T>::operator==(const _Box3& other)
	{
		return !(position != other.position || size != other.size);
	}

	template<typename T>
	inline bool _Box3<T>::operator!=(const _Box3& other)
	{
		return position != other.position || size != other.size;

	}



	// Typedefs

	typedef _Box2<float> Box2;
	typedef _Box2<double> Box2d;
	typedef _Box2<int> Box2i;
	typedef _Box2<unsigned int> Box2u;

	typedef _Box3<float> Box3;
	typedef _Box3<double> Box3d;
	typedef _Box3<int> Box3i;
	typedef _Box3<unsigned int> Box3u;

}

#ifdef CHSL_LINEAR

using cs::Box2;
using cs::Box2d;
using cs::Box2i;
using cs::Box2u;

using cs::Box3;
using cs::Box3d;
using cs::Box3i;
using cs::Box3u;

#endif
