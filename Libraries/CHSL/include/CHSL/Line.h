#pragma once

#include "Vector.h"
#include "Debug.h"

namespace cs
{

	template<typename T>
	class _Line2
	{
	public:
		_Line2(_Vec2<T> origin, _Vec2<T> direction);
		_Line2(const _Line2&);
		_Line2(_Line2&&);

		_Vec2<T> operator()(T t) const;

		_Line2& operator=(const _Line2&);
		_Line2& operator=(_Line2&&);

		bool operator==(const _Line2&) const;
		bool operator!=(const _Line2&) const;

		template<typename T2>
		operator _Line2<T2>();

		// Whether the lines are parallel and overlapping
		bool Equivalent(const _Line2<T>& other) const;
		bool Parallel(const _Line2<T>& other) const;
		bool Contains(const _Vec2<T>& vector) const;
		
		// Returns the intersection point
		bool Intersection(const _Line2<T>& otherLine, _Vec2<T>& out) const;
		bool IntersectionT(const _Line2<T>& otherLine, T& out) const;

		_Vec2<T> ClosestPoint(const _Vec2<T>& point) const;
		T ClosestPointT(const _Vec2<T>& point) const;

		const _Vec2<T>& GetOrigin() const;
		const _Vec2<T>& GetDirection() const;

		void SetOrigin(const _Vec2<T>& origin);
		void SetDirection(const _Vec2<T>& direction);

	private:
		_Vec2<T> m_origin;
		_Vec2<T> m_direction;
	};

	template<typename T>
	class _Line3
	{
	public:
		_Line3(_Vec3<T> origin, _Vec3<T> direction);
		_Line3(const _Line3&);
		_Line3(_Line3&&);

		template<typename T2>
		operator _Line3<T2>();

		_Vec3<T> operator()(T t) const;

		_Line3& operator=(const _Line3&);
		_Line3& operator=(_Line3&&);

		bool operator==(const _Line3&) const;
		bool operator!=(const _Line3&) const;

		// Whether the lines are parallel and overlapping
		bool Equivalent(const _Line3<T>& other) const;
		bool Parallel(const _Line3<T>& other) const;
		bool Contains(const _Vec3<T>& vector) const;

		_Vec3<T> ClosestPoint(const _Vec3<T>& point) const;
		T ClosestPointT(const _Vec3<T>& point) const;

		const _Vec3<T>& GetOrigin() const;
		const _Vec3<T>& GetDirection() const;

		void SetOrigin(const _Vec3<T>& origin);
		void SetDirection(const _Vec3<T>& direction);

	private:
		_Vec3<T> m_origin;
		_Vec3<T> m_direction;
	};



	typedef _Line2<float> Line2;
	typedef _Line2<double> Line2d;
	typedef _Line2<int> Line2i;

	typedef _Line3<float> Line3;
	typedef _Line3<double> Line3d;
	typedef _Line3<int> Line3i;



	// Definition

	template<typename T>
	inline _Line2<T>::_Line2(_Vec2<T> origin, _Vec2<T> direction)
		:
		m_origin(origin),
		m_direction(direction)
	{
		if (direction.x == 0 && direction.y == 0)
		{
			throw ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Invalid _Line2 intialization with direction == { 0, 0 }.");
		}
	}

	template<typename T>
	_Line2<T>::_Line2(const _Line2& lVal)
		:
		m_origin(lVal.m_origin),
		m_direction(lVal.m_direction)
	{
	}

	template<typename T>
	_Line2<T>::_Line2(_Line2&& rVal)
		:
		m_origin(rVal.m_origin),
		m_direction(rVal.m_direction)
	{
	}

	template<typename T>
	cs::_Vec2<T> _Line2<T>::operator()(T t) const
	{
		return m_origin + m_direction * t;
	}

	template<typename T>
	cs::_Line2<T>& _Line2<T>::operator=(const _Line2& lVal)
	{
		m_origin = lVal.m_origin;
		m_direction = lVal.m_direction;

		return *this;
	}

	template<typename T>
	cs::_Line2<T>& _Line2<T>::operator=(_Line2&& rVal)
	{
		m_origin = rVal.m_origin;
		m_direction = rVal.m_direction;

		return *this;
	}

	template<typename T>
	bool _Line2<T>::operator==(const _Line2& other) const
	{
		return m_origin == other.m_origin && m_direction == other.m_direction;
	}

	template<typename T>
	bool _Line2<T>::operator!=(const _Line2& other) const
	{
		return m_origin != other.m_origin || m_direction != other.m_direction;
	}

	template<typename T>
	bool _Line2<T>::Equivalent(const _Line2<T>& other) const
	{
		return Parallel(other) && Contains(other.m_origin);
	}

	template<typename T>
	bool _Line2<T>::Parallel(const _Line2<T>& other) const
	{
		return m_direction.Parallel(other.m_direction);
	}

	template<typename T>
	bool _Line2<T>::Contains(const _Vec2<T>& vector) const
	{
		return m_direction.Parallel(m_origin - vector);
	}

	template<typename T>
	bool _Line2<T>::Intersection(const _Line2<T>& otherLine, _Vec2<T>& out) const
	{
		T t;
		bool intersect = IntersectionT(otherLine, t);

		if (!intersect)
		{
			return false;
		}

		out = (*this)(t);

		return true;
	}

	template<typename T>
	bool _Line2<T>::IntersectionT(const _Line2<T>& otherLine, T& out) const
	{
		if (Parallel(otherLine))
		{
			return false;
		}

		out = (otherLine.m_direction.y * (otherLine.m_origin.x - m_origin.x) + m_origin.y - otherLine.m_origin.y)
			/ (m_direction.x * otherLine.m_direction.y - m_direction.y);

		return true;
	}

	template<typename T>
	_Vec2<T> _Line2<T>::ClosestPoint(const _Vec2<T>& point) const
	{
		return (*this)(ClosestPointT(point));
	}

	template<typename T>
	T _Line2<T>::ClosestPointT(const _Vec2<T>& point) const
	{
		return (m_direction.x * (point.x - m_origin.x) + m_direction.y * (point.y - m_origin.y))
			/ (m_direction.x * m_direction.x + m_direction.y * m_direction.y);
	}

	template<typename T>
	const _Vec2<T>& _Line2<T>::GetOrigin() const
	{
		return m_origin;
	}

	template<typename T>
	const _Vec2<T>& _Line2<T>::GetDirection() const
	{
		return m_direction;
	}

	template<typename T>
	void _Line2<T>::SetOrigin(const _Vec2<T>& origin)
	{
		m_origin = origin;
	}

	template<typename T>
	void _Line2<T>::SetDirection(const _Vec2<T>& direction)
	{
		if (direction.x == 0 && direction.y == 0)
		{
			throw ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Invalid setting of line direction == { 0, 0 }.");
		}

		m_direction = direction;
	}

	template<typename T>
	template<typename T2>
	_Line2<T>::operator cs::_Line2<T2>()
	{
		return _Line2<T2>(
			_Vec2<T2>((T2)m_origin.x, (T2)m_origin.y),
			_Vec2<T2>((T2)m_direction.x, (T2)m_direction.y)
		);
	}



	// Definition _Line3

	template<typename T>
	inline _Line3<T>::_Line3(_Vec3<T> origin, _Vec3<T> direction)
		:
		m_origin(origin),
		m_direction(direction)
	{
		if (direction.x == 0 && direction.y == 0 && direction.z == 0)
		{
			throw ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Invalid _Line3 intialization with direction == { 0, 0 }.");
		}
	}

	template<typename T>
	_Line3<T>::_Line3(const _Line3& lVal)
		:
		m_origin(lVal.m_origin),
		m_direction(lVal.m_direction)
	{
	}

	template<typename T>
	_Line3<T>::_Line3(_Line3&& rVal)
		:
		m_origin(rVal.m_origin),
		m_direction(rVal.m_direction)
	{
	}

	template<typename T>
	cs::_Vec3<T> _Line3<T>::operator()(T t) const
	{
		return m_origin + m_direction * t;
	}

	template<typename T>
	cs::_Line3<T>& _Line3<T>::operator=(const _Line3& lVal)
	{
		m_origin = lVal.m_origin;
		m_direction = lVal.m_direction;

		return *this;
	}

	template<typename T>
	cs::_Line3<T>& _Line3<T>::operator=(_Line3&& rVal)
	{
		m_origin = rVal.m_origin;
		m_direction = rVal.m_direction;

		return *this;
	}

	template<typename T>
	bool _Line3<T>::operator==(const _Line3& other) const
	{
		return m_origin == other.m_origin && m_direction == other.m_direction;
	}

	template<typename T>
	bool _Line3<T>::operator!=(const _Line3& other) const
	{
		return m_origin != other.m_origin || m_direction != other.m_direction;
	}

	template<typename T>
	bool _Line3<T>::Equivalent(const _Line3<T>& other) const
	{
		return Parallel(other) && Contains(other.m_origin);
	}

	template<typename T>
	bool _Line3<T>::Parallel(const _Line3<T>& other) const
	{
		return m_direction.Parallel(other.m_direction);
	}

	template<typename T>
	bool _Line3<T>::Contains(const _Vec3<T>& vector) const
	{
		return m_direction.Parallel(m_origin - vector);
	}

	template<typename T>
	_Vec3<T> _Line3<T>::ClosestPoint(const _Vec3<T>& point) const
	{
		return (*this)(ClosestPointT(point));
	}

	template<typename T>
	T _Line3<T>::ClosestPointT(const _Vec3<T>& point) const
	{
		return (m_direction.x * (point.x - m_origin.x) + m_direction.y * (point.y - m_origin.y) + m_direction.z * (point.z - m_origin.z))
			/ (m_direction.x * m_direction.x + m_direction.y * m_direction.y + m_direction.z * m_direction.z);
	}

	template<typename T>
	const _Vec3<T>& _Line3<T>::GetOrigin() const
	{
		return m_origin;
	}

	template<typename T>
	const _Vec3<T>& _Line3<T>::GetDirection() const
	{
		return m_direction;
	}

	template<typename T>
	void _Line3<T>::SetOrigin(const _Vec3<T>& origin)
	{
		m_origin = origin;
	}

	template<typename T>
	void _Line3<T>::SetDirection(const _Vec3<T>& direction)
	{
		if (direction.x == 0 && direction.y == 0 && direction.z == 0)
		{
			throw ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Invalid setting of line direction == { 0, 0 }.");
		}

		m_direction = direction;
	}

	template<typename T>
	template<typename T2>
	_Line3<T>::operator cs::_Line3<T2>()
	{
		return _Line3<T2>(
			_Vec3<T2>((T2)m_origin.x, (T2)m_origin.y, (T2)m_origin.z),
			_Vec3<T2>((T2)m_direction.x, (T2)m_direction.y, (T2)m_direction.z)
		);
	}

}


#ifdef CHSL_LINEAR

using cs::Line2;
using cs::Line2d;
using cs::Line2i;
using cs::Line3;
using cs::Line3d;
using cs::Line3i;

#endif