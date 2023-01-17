#pragma once

#include "Vector.h"

namespace cs
{

	template<typename T>
	class _Rectangle
	{
	public:
		_Rectangle();
		_Rectangle(cs::_Vec2<T> position, cs::_Vec2<T> size);
		_Rectangle(T x, T y, T width, T height);

		template<typename T2>
		constexpr operator _Rectangle<T2>();

		// Intersects or touches other rectangle (can share an edge)
		constexpr bool Touches(const _Rectangle& other) const;		
		// Strictly intersects other rectangle (cannot merely share an edge)
		constexpr bool Overlaps(const _Rectangle& other) const;
		// Wholly contains other rectangle (can share edges)
		constexpr bool Contains(const _Rectangle& other) const;	
		// Strictly contains other rectangle (larger on all sides)
		constexpr bool Envelops(const _Rectangle& other) const;		

		// Contains vector/point (can be on edge)
		constexpr bool Contains(const _Vec2<T>& vector) const;		
		// Wholly contains vector/point (cannot be on edge)
		constexpr bool Envelops(const _Vec2<T>& vector) const;
		// Same as Contains
		constexpr bool Touches(const _Vec2<T>& vector) const; 
		// Same as Envelops
		constexpr bool Overlaps(const _Vec2<T>& vector) const; 

		_Rectangle& operator=(const _Rectangle& other);
		constexpr bool operator==(const _Rectangle& other);
		constexpr bool operator!=(const _Rectangle& other);

	public:
		cs::_Vec2<T> position;
		cs::_Vec2<T> size;
	};



	// Implementation

	template<typename T>
	inline _Rectangle<T>::_Rectangle()
		:
		position(0, 0),
		size(0, 0)
	{
	}

	template<typename T>
	inline _Rectangle<T>::_Rectangle(cs::_Vec2<T> position, cs::_Vec2<T> size)
		:
		position(position),
		size(size)
	{
	}

	template<typename T>
	inline _Rectangle<T>::_Rectangle(T x, T y, T width, T height)
		:
		position(x, y),
		size(width, height)
	{
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::Touches(const _Rectangle& other) const
	{
		return !(
			other.position.x + other.size.x < position.x ||
			other.position.y + other.size.y < position.y ||
			other.position.x > position.x + size.x ||
			other.position.y > position.y + size.y
		);
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::Overlaps(const _Rectangle& other) const
	{
		return !(
			other.position.x + other.size.x <= position.x ||
			other.position.y + other.size.y <= position.y ||
			other.position.x >= position.x + size.x ||
			other.position.y >= position.y + size.y
		);
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::Contains(const _Rectangle& other) const
	{
		return !(
			other.position.x < position.x ||
			other.position.y < position.y ||
			other.position.x + other.size.x > position.x + size.x ||
			other.position.y + other.size.y > position.y + size.y
		);
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::Contains(const _Vec2<T>& vector) const
	{
		return !(
			vector.x < position.x ||
			vector.y < position.y ||
			vector.x > position.x + size.x ||
			vector.y > position.y + size.y
		);
	}

	template<typename T>
	inline constexpr bool _Rectangle<T>::Envelops(const _Rectangle& other) const
	{
		return !(
			other.position.x <= position.x ||
			other.position.y <= position.y ||
			other.position.x + other.size.x >= position.x + size.x ||
			other.position.y + other.size.y >= position.y + size.y
		);
	}

	template<typename T>
	inline constexpr bool _Rectangle<T>::Envelops(const _Vec2<T>& vector) const
	{
		return !(
			vector.x <= position.x ||
			vector.y <= position.y ||
			vector.x >= position.x + size.x ||
			vector.y >= position.y + size.y
		);
	}

	template<typename T>
	inline constexpr bool _Rectangle<T>::Touches(const _Vec2<T>& vector) const
	{
		return !(
			vector.x < position.x ||
			vector.y < position.y ||
			vector.x > position.x + size.x ||
			vector.y > position.y + size.y
		);
	}

	template<typename T>
	inline constexpr bool _Rectangle<T>::Overlaps(const _Vec2<T>& vector) const
	{
		return !(
			vector.x <= position.x ||
			vector.y <= position.y ||
			vector.x >= position.x + size.x ||
			vector.y >= position.y + size.y
			);
	}

	template<typename T>
	inline _Rectangle<T>& _Rectangle<T>::operator=(const _Rectangle& other)
	{
		position = other.position;
		size = other.size;

		return *this;
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::operator==(const _Rectangle& other)
	{
		return position == other.position && size == other.size;
	}

	template<typename T>
	constexpr inline bool _Rectangle<T>::operator!=(const _Rectangle& other)
	{
		return position != other.position || size != other.size;
	}

	template<typename T>
	template<typename T2>
	constexpr inline _Rectangle<T>::operator _Rectangle<T2>()
	{
		return _Rectangle<T2>((T2)position.x, (T2)position.y, (T2)size.x, (T2)size.y);
	}



	// Typedefs

	typedef _Rectangle<float> Rect;
	typedef _Rectangle<int> IRect;
	typedef _Rectangle<unsigned int> URect;

}

#ifdef CHSL_LINEAR

using cs::Rect;
using cs::IRect;
using cs::URect;

#endif