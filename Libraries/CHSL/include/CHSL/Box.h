#pragma once

#include "IRaycastTarget.h"
#include "Vector.h"

namespace cs
{

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

	template<typename T>
	struct _Box
	{
	public:
		_Box();
		_Box(cs::_Vec3<T> position, cs::_Vec3<T> size);
		_Box(T x, T y, T z, T width, T height, T depth);
		_Box(const _Box&);

		// Intersects or touches other box (can share an edge)
		constexpr bool Touches(const _Box& other) const;
		// Strictly intersects other box (cannot merely share an edge)
		constexpr bool Overlaps(const _Box& other) const;
		// Wholly contains other box (can share edges)
		constexpr bool Contains(const _Box& other) const;
		// Strictly contains other box (larger on all sides)
		constexpr bool Envelops(const _Box& other) const;

		// Contains vector/point (can be on edge)
		constexpr bool Contains(const _Vec3<T>& vector) const;
		// Wholly contains vector/point (cannot be on edge)
		constexpr bool Envelops(const _Vec3<T>& vector) const;
		// Same as Contains
		constexpr bool Touches(const _Vec3<T>& vector) const;
		// Same as Envelops
		constexpr bool Overlaps(const _Vec3<T>& vector) const;

		_Box& operator=(const _Box& other);
		bool operator==(const _Box& other);
		bool operator!=(const _Box& other);

	public:
		cs::_Vec3<T> position;
		cs::_Vec3<T> size;
	};



	// Implementation

	template<typename T>
	inline _Box<T>::_Box()
		:
		position(),
		size()
	{
	}

	template<typename T>
	inline _Box<T>::_Box(cs::_Vec3<T> position, cs::_Vec3<T> size)
		:
		position(position),
		size(size)
	{
	}

	template<typename T>
	inline _Box<T>::_Box(T x, T y, T z, T width, T height, T depth)
		:
		position(x, y, z),
		size(width, height, depth)
	{
	}

	template<typename T>
	inline _Box<T>::_Box(const _Box& lVal)
		:
		position(lVal.position),
		size(lVal.size)
	{
	}


	template<typename T>
	constexpr inline bool _Box<T>::Touches(const _Box& other) const
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
	constexpr inline bool _Box<T>::Overlaps(const _Box& other) const
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
	constexpr inline bool _Box<T>::Contains(const _Box& other) const
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
	inline constexpr bool _Box<T>::Envelops(const _Box& other) const
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
	constexpr inline bool _Box<T>::Contains(const _Vec3<T>& vector) const
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
	inline constexpr bool _Box<T>::Envelops(const _Vec3<T>& vector) const
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
	inline constexpr bool _Box<T>::Touches(const _Vec3<T>& vector) const
	{
		return Contains(vector);
	}

	template<typename T>
	inline constexpr bool _Box<T>::Overlaps(const _Vec3<T>& vector) const
	{
		return Overlaps(vector);
	}

	template<typename T>
	inline _Box<T>& _Box<T>::operator=(const _Box& other)
	{
		position = other.position;
		size = other.size;
		return *this;
	}

	template<typename T>
	inline bool _Box<T>::operator==(const _Box& other)
	{
		return !(position != other.position || size != other.size);
	}

	template<typename T>
	inline bool _Box<T>::operator!=(const _Box& other)
	{
		return position != other.position || size != other.size;

	}



	// Typedefs

	typedef _Box<float> Box;
	typedef _Box<int> IBox;
	typedef _Box<unsigned int> UBox;

}

#ifdef CHSL_LINEAR

using cs::Box;
using cs::IBox;
using cs::UBox;

#endif
