#pragma once

/* CHSL

	|	_Vec2<T>	(Point2, Point2u, Vec2, Vec2d)
	|	_Vec3<T>	(Point3, Point3u, Vec3, Vec3d)
	|	_Vec4<T>	(Point4, Point4u, Vec4, Vec4d)
	|
	|	- Mathematical vector implementation.
	|	- Foundational piece of the CHSL linear algebra system.

*/









#include <cmath> 

namespace cs
{


	template<typename T>
	class _Vec2
	{
	public:
		T x, y;

	public:
		_Vec2();
		_Vec2(T X, T Y);
		_Vec2(const _Vec2& copy);

		float LengthSq();
		float Length();

		_Vec2& Normalize();
		_Vec2 Normalized() const;

		bool Parallel(const _Vec2& other) const;

		_Vec2 Componetwise(const _Vec2& other) const;
		T Dot(const _Vec2& other) const;

#ifdef CHSL_DX
		DirectX::XMVECTOR GetXM2(float z = 0.0f, float w = 0.0f);
		_Vec2& Rotate(float angle);
#endif

		// Conversion

		template<typename T2>
		explicit operator _Vec2<T2>() const;



		// Copy

		_Vec2<T> operator-() const;

		_Vec2<T> operator+(const _Vec2& b) const;
		_Vec2<T> operator-(const _Vec2& b) const;
		_Vec2<T> operator%(const _Vec2& b) const;
		T operator*(const _Vec2& b) const;

		_Vec2<T> operator*(const T& b) const;
		_Vec2<T> operator/(const T& b) const;



		// Assignment

		_Vec2<T>& operator+=(const _Vec2& b);
		_Vec2<T>& operator-=(const _Vec2& b);
		_Vec2<T>& operator&=(const _Vec2& b);

		_Vec2<T>& operator*=(const T& b);
		_Vec2<T>& operator/=(const T& b);



		// Evaluation

		bool operator==(const _Vec2& b) const;
		bool operator!=(const _Vec2& b) const;
	};





	template<typename T>
	class _Vec3
	{
	public:
		T x;
		T y;
		T z;
		
	public:
		_Vec3();
		_Vec3(T X, T Y, T Z);
		_Vec3(const _Vec3& copy);
		_Vec3(const _Vec2<T>& downsample);

		float LengthSq() const;
		float Length() const;

		_Vec3& Normalize();
		_Vec3 Normalized() const;

		bool Parallel(const _Vec3& other) const;

		_Vec3 Cross(const _Vec3& other) const;
		_Vec3 Componentwise(const _Vec3& other) const;
		T Dot(const _Vec3& other) const;

#ifdef CHSL_DX
		DirectX::XMVECTOR GetXM3(float w = 1.0f);

		_Vec3& RotateX(float angle);
		_Vec3& RotateY(float angle);
		_Vec3& RotateZ(float angle);
		_Vec3& Rotate3D(const _Vec3& rotation);
#endif


		// Conversion

		template<typename T2>
		explicit operator _Vec3<T2>() const;

		operator _Vec2<T>() const;


		// Copy

		_Vec3 operator-() const;

		_Vec3 operator+(const _Vec3& b) const;
		_Vec3 operator-(const _Vec3& b) const; 
		_Vec3 operator%(const _Vec3& b) const;  // Componentwise multiplication
		_Vec3 operator^(const _Vec3& b) const;  // Cross product
		T operator*(const _Vec3& b) const;		// Dot product

		_Vec3 operator*(const T& b) const; 
		_Vec3 operator/(const T& b) const; 


		// Assignment

		_Vec3& operator=(const _Vec3& b);

		_Vec3& operator+=(const _Vec3& b);
		_Vec3& operator-=(const _Vec3& b);
		_Vec3& operator&=(const _Vec3& b);
		_Vec3& operator^=(const _Vec3& b);

		_Vec3& operator*=(const T& b);
		_Vec3& operator/=(const T& b);


		// Evaluation

		bool operator==(const _Vec3& b) const;
		bool operator!=(const _Vec3& b) const;
	};





	template<typename T>
	class _Vec4
	{
	public:
		T x;
		T y;
		T z;
		T w;

	public:
		_Vec4();
		_Vec4(T X, T Y, T Z, T W);
		_Vec4(const _Vec4& copy);
		_Vec4(const _Vec3<T>& downsample, T W = 1);

		float LengthSq() const;
		float Length() const;

		_Vec4 Componentwise(const _Vec4& other) const;
		T Dot(const _Vec4& other) const;

#ifdef CHSL_DX
		DirectX::XMVECTOR GetXM3();
#endif


		// Conversion

		template<typename T2>
		explicit operator _Vec4<T2>() const;

		operator _Vec3<T>() const;


		// Copy

		_Vec4 operator-() const;

		_Vec4 operator+(const _Vec4& b) const;
		_Vec4 operator-(const _Vec4& b) const;
		_Vec4 operator%(const _Vec4& b) const;
		T operator*(const _Vec4& b) const;

		_Vec4 operator*(const T& b) const;
		_Vec4 operator/(const T& b) const;


		// Assignment

		_Vec4& operator=(const _Vec4& b);

		_Vec4& operator+=(const _Vec4& b);
		_Vec4& operator-=(const _Vec4& b);
		_Vec4& operator&=(const _Vec4& b);

		_Vec4& operator*=(const T& b);
		_Vec4& operator/=(const T& b);


		// Evaluation

		bool operator==(const _Vec4& b) const;
		bool operator!=(const _Vec4& b) const;
	};





	// ------------------------------------------------ Vec2 implementation

	template<typename T>
	inline _Vec2<T>::_Vec2()
	{
		x = 0;
		y = 0;
	}

	template<typename T>
	inline _Vec2<T>::_Vec2(T X, T Y)
	{
		x = X;
		y = Y;
	}

	template<typename T>
	inline _Vec2<T>::_Vec2(const _Vec2& copy)
	{
		x = copy.x;
		y = copy.y;
	}

	template<typename T>
	inline float _Vec2<T>::LengthSq()
	{
		return ((float)x) * x + ((float)y) * y;
		}

	template<typename T>
	inline float _Vec2<T>::Length()
	{
		return std::sqrtf(LengthSq());
	}

	template<typename T>
	inline _Vec2<T>& _Vec2<T>::Normalize()
	{
		float tempLength = Length();
		(x /= (T)tempLength);
		(y /= (T)tempLength);
		return *this;
	}

	template<typename T>
	inline _Vec2<T> _Vec2<T>::Normalized() const
	{
		return _Vec2(*this).Normalize();
	}

	template<typename T>
	inline bool _Vec2<T>::Parallel(const _Vec2& other) const
	{
		if (other.x != 0)
		{
			return other.y * (x / other.x) == y;
		}
		else if (other.y != 0)
		{
			return x == 0 && y != 0;
		}

		return x == 0 && y == 0;
	}

	template<typename T>
	inline _Vec2<T> _Vec2<T>::Componetwise(const _Vec2& other) const
	{
		return _Vec2(x * other.x, y * other.y);
	}

	template<typename T>
	inline T _Vec2<T>::Dot(const _Vec2& other) const
	{
		return x * other.x + y * other.y;
	}


#ifdef CHSL_DX
	template<typename T>
	inline DirectX::XMVECTOR _Vec2<T>::GetXM2(float z, float w)
	{
		return DirectX::XMVectorSet((float)x, (float)y, z, w);
	}

	template<typename T>
	inline _Vec2<T>& _Vec2<T>::Rotate(float angle)
	{
		dx::XMVECTOR vec = dx::XMVector3Transform(GetXM2(), dx::XMMatrixRotationZ(-angle));

		x = (T)dx::XMVectorGetX(vec);
		y = (T)dx::XMVectorGetY(vec);

		return *this;
	}
#endif

	template<typename T>
	template<typename T2>
	inline _Vec2<T>::operator _Vec2<T2>() const
	{
		return _Vec2<T2>((T2)x, (T2)y);
	}


	// Copy

	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator-() const
	{
		return _Vec2(-x, -y);
	}

	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator+(const _Vec2& b) const
	{
		return _Vec2(x + b.x, y + b.y);
	}
	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator-(const _Vec2& b) const
	{
		return _Vec2(x - b.x, y - b.y);
	}
	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator%(const _Vec2& b) const
	{
		return _Vec2(x * b.x, y * b.y);
	}
	template<typename T>
	inline T _Vec2<T>::operator*(const _Vec2& b) const
	{
		return x * b.x + y * b.y;
	}

	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator*(const T& b) const
	{
		return _Vec2(x * b, y * b);
	}
	template<typename T>
	inline _Vec2<T> _Vec2<T>::operator/(const T& b) const
	{
		return _Vec2(x / b, y / b);
	}

	// Assignment

	template<typename T>
	inline _Vec2<T>& _Vec2<T>::operator+=(const _Vec2& b)
	{
		x += b.x;
		y += b.y;

		return *this;
	}
	template<typename T>
	_Vec2<T>& _Vec2<T>::operator-=(const _Vec2& b)
	{
		x -= b.x;
		y -= b.y;

		return *this;
	}
	template<typename T>
	inline _Vec2<T>& _Vec2<T>::operator&=(const _Vec2& b)
	{
		x *= b.x;
		y *= b.y;

		return *this;
	}

	template<typename T>
	inline _Vec2<T>& _Vec2<T>::operator*=(const T& b)
	{
		x *= b;
		y *= b;

		return *this;
	}
	template<typename T>
	inline _Vec2<T>& _Vec2<T>::operator/=(const T& b)
	{
		x /= b;
		y /= b;

		return *this;
	}

	// Relational

	template<typename T>
	inline bool _Vec2<T>::operator==(const _Vec2& b) const
	{
		return x == b.x && y == b.y;
	}
	template<typename T>
	inline bool _Vec2<T>::operator!=(const _Vec2& b) const
	{
		return x != b.x || y != b.y;
	}





	// ----------------------------------------------- Vec3 definition

	template<typename T>
	inline _Vec3<T>::_Vec3()
		:
		x(0),
		y(0),
		z(0)
	{
	}

	template<typename T>
	inline _Vec3<T>::_Vec3(T X, T Y, T Z)
		:
		x(X),
		y(Y),
		z(Z)
	{
	}

	template<typename T>
	inline _Vec3<T>::_Vec3(const _Vec3& copy)
		:
		x(copy.x),
		y(copy.y),
		z(copy.z)
	{
	}

	template<typename T>
	inline _Vec3<T>::_Vec3(const _Vec2<T>& downsample)
		:
		x(downsample.x),
		y(downsample.y),
		z(0)
	{
	}

	template<typename T>
	inline float _Vec3<T>::LengthSq() const
	{
		return ((float)x) * x + ((float)y) * y + ((float)z) * z;
	}

	template<typename T>
	inline float _Vec3<T>::Length() const
	{
		return std::sqrtf(LengthSq());
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::Normalize()
	{
		float tempLength = Length();
		x /= tempLength;
		y /= tempLength;
		z /= tempLength;
		return *this;
	}

	template<typename T>
	inline _Vec3<T> _Vec3<T>::Normalized() const
	{
		return _Vec3(*this).Normalize();
	}

	template<typename T>
	inline bool _Vec3<T>::Parallel(const _Vec3& other) const
	{
		_Vec3 cross = Cross(other);

		return cross.x == 0 && cross.y == 0 && cross.z == 0;
	}

	template<typename T>
	inline _Vec3<T> _Vec3<T>::Cross(const _Vec3& other) const
	{
		return _Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	template<typename T>
	inline _Vec3<T> _Vec3<T>::Componentwise(const _Vec3& other) const
	{
		return _Vec3(x * other.x, y * other.y, z * other.z);
	}

	template<typename T>
	inline T _Vec3<T>::Dot(const _Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

#ifdef CHSL_DX

	template<typename T>
	inline DirectX::XMVECTOR _Vec3<T>::GetXM3(float w)
	{
		return DirectX::XMVectorSet((float)x, (float)y, (float)z, w);
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::RotateX(float angle)
	{
		dx::XMVECTOR vec = dx::XMVector3Transform(GetXM3(), dx::XMMatrixRotationX(-angle));

		y = (T)dx::XMVectorGetY(vec);
		z = (T)dx::XMVectorGetZ(vec);

		return *this;
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::RotateY(float angle)
	{
		dx::XMVECTOR vec = dx::XMVector3Transform(GetXM3(), dx::XMMatrixRotationY(-angle));

		x = (T)dx::XMVectorGetX(vec);
		z = (T)dx::XMVectorGetZ(vec);

		return *this;
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::RotateZ(float angle)
	{
		dx::XMVECTOR vec = dx::XMVector3Transform(GetXM3(), dx::XMMatrixRotationZ(-angle));

		x = (T)dx::XMVectorGetX(vec);
		y = (T)dx::XMVectorGetY(vec);

		return *this;
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::Rotate3D(const _Vec3& rotation)
	{
		_Vec3 temp = -rotation;

		dx::XMVECTOR vec = dx::XMVector3Transform(GetXM3(), dx::XMMatrixRotationRollPitchYawFromVector(temp.GetXM3()));

		x = (T)dx::XMVectorGetX(vec);
		y = (T)dx::XMVectorGetY(vec);
		z = (T)dx::XMVectorGetZ(vec);

		return *this;
	}

#endif

	template<typename T>
	template<typename T2>
	inline _Vec3<T>::operator _Vec3<T2>() const
	{
		return _Vec3<T2>((T2)x, (T2)y, (T2)z);
	}

	template<typename T>
	inline _Vec3<T>::operator _Vec2<T>() const
	{
		return _Vec2<T>(x, y);
	}

	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator-() const
	{
		return _Vec3(-x, -y, -z);
	}

	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator+(const _Vec3& b) const
	{
		return _Vec3(x + b.x, y + b.y, z + b.z);
	}
	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator-(const _Vec3& b) const
	{
		return _Vec3(x - b.x, y - b.y, z - b.z);
	}
	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator%(const _Vec3& b) const
	{
		return _Vec3(x * b.x, y * b.y, z * b.z);
	}
	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator^(const _Vec3& b) const
	{
		return _Vec3(
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x
		);
	}
	template<typename T>
	T _Vec3<T>::operator*(const _Vec3& b) const
	{
		return 
			x * b.x + 
			y * b.y + 
			z * b.z;
	}


	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator*(const T& b) const
	{
		return _Vec3(x * b, y * b, z * b);
	}
	template<typename T>
	inline _Vec3<T> _Vec3<T>::operator/(const T& b) const
	{
		return _Vec3(x / b, y / b, z / b);
	}

	// Assignment

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator=(const _Vec3& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;

		return *this;
	}

	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator+=(const _Vec3& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;

		return *this;
	}
	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator-=(const _Vec3& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;

		return *this;
	}
	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator&=(const _Vec3& b)
	{
		x *= b.x;
		y *= b.y;
		z *= b.z;

		return *this;
	}
	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator^=(const _Vec3& b)
	{
		T tempX = y * b.z - z * b.y;
		T tempY = z * b.x - x * b.z;

		z = x * b.y - y * b.x;
		x = tempX;
		y = tempY;
		
		return *this;
	}
	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator*=(const T& b)
	{
		x *= b;
		y *= b;
		z *= b;

		return *this;
	}
	template<typename T>
	inline _Vec3<T>& _Vec3<T>::operator/=(const T& b)
	{
		x /= b;
		y /= b;
		z /= b;

		return *this;
	}

	// Evaluation

	template<typename T>
	inline bool _Vec3<T>::operator==(const _Vec3& b) const
	{
		return x == b.x && y == b.y && z == b.z;
	}
		
	template<typename T>
	inline bool _Vec3<T>::operator!=(const _Vec3& b) const
	{
		return x != b.x || y != b.y || z != b.z;
	}





	// ----------------------------------------------- Vec4 definition

	template<typename T>
	inline _Vec4<T>::_Vec4()
		:
		x(0),
		y(0),
		z(0),
		w(0)
	{
	}

	template<typename T>
	inline _Vec4<T>::_Vec4(T X, T Y, T Z, T W)
		:
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	template<typename T>
	inline _Vec4<T>::_Vec4(const _Vec4& copy)
		:
		x(copy.x),
		y(copy.y),
		z(copy.z),
		w(copy.w)
	{
	}

	template<typename T>
	inline _Vec4<T>::_Vec4(const _Vec3<T>& downsample, T W)
		:
		x(downsample.x),
		y(downsample.y),
		z(downsample.z),
		w(W)
	{
	}

	template<typename T>
	inline float _Vec4<T>::LengthSq() const
	{
		return ((float)x) * x + ((float)y) * y + ((float)z) * z + ((float)w) * w;
	}

	template<typename T>
	inline float _Vec4<T>::Length() const
	{
		return std::sqrtf(LengthSq());
	}

	template<typename T>
	inline _Vec4<T> _Vec4<T>::Componentwise(const _Vec4& other) const
	{
		return _Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
	}

	template<typename T>
	inline T _Vec4<T>::Dot(const _Vec4& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

#ifdef CHSL_DX

	template<typename T>
	inline DirectX::XMVECTOR _Vec4<T>::GetXM3()
	{
		return DirectX::XMVectorSet((float)x, (float)y, (float)z, (float)w);
	}

#endif

	template<typename T>
	template<typename T2>
	inline _Vec4<T>::operator _Vec4<T2>() const
	{
		return _Vec4<T2>((T2)x, (T2)y, (T2)z, (T2)w);
	}

	template<typename T>
	inline _Vec4<T>::operator _Vec3<T>() const
	{
		return _Vec3<T>(x, y, z);
	}

	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator-() const
	{
		return _Vec4(-x, -y, -z, -w);
	}

	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator+(const _Vec4& b) const
	{
		return _Vec4(x + b.x, y + b.y, z + b.z, w + b.w);
	}
	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator-(const _Vec4& b) const
	{
		return _Vec4(x - b.x, y - b.y, z - b.z, w - b.w);
	}
	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator%(const _Vec4& b) const
	{
		return _Vec4(x * b.x, y * b.y, z * b.z, w * b.w);
	}
	template<typename T>
	T _Vec4<T>::operator*(const _Vec4& b) const
	{
		return
			x * b.x +
			y * b.y +
			z * b.z +
			w * b.w;
	}


	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator*(const T& b) const
	{
		return _Vec4(x * b, y * b, z * b, w * b);
	}
	template<typename T>
	inline _Vec4<T> _Vec4<T>::operator/(const T& b) const
	{
		return _Vec4(x / b, y / b, z / b, w / b);
	}

	// Assignment

	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator=(const _Vec4& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		w = b.w;

		return *this;
	}

	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator+=(const _Vec4& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		w += b.w;

		return *this;
	}
	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator-=(const _Vec4& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		w -= b.w;

		return *this;
	}
	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator&=(const _Vec4& b)
	{
		x *= b.x;
		y *= b.y;
		z *= b.z;
		w *= b.w;

		return *this;
	}
	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator*=(const T& b)
	{
		x *= b;
		y *= b;
		z *= b;
		w *= b;

		return *this;
	}
	template<typename T>
	inline _Vec4<T>& _Vec4<T>::operator/=(const T& b)
	{
		x /= b;
		y /= b;
		z /= b;
		w /= b;

		return *this;
	}

	// Evaluation

	template<typename T>
	inline bool _Vec4<T>::operator==(const _Vec4& b) const
	{
		return x == b.x && y == b.y && z == b.z && w == b.w;
	}

	template<typename T>
	inline bool _Vec4<T>::operator!=(const _Vec4& b) const
	{
		return x != b.x || y != b.y || z != b.z || w != b.w;
	}





	// ------------------------------------------------- Postface

	typedef _Vec2<int> Point2;
	typedef _Vec2<unsigned int> Point2u;
	typedef _Vec2<float> Vec2;
	typedef _Vec2<double> Vec2d;

	typedef _Vec3<int> Point3;
	typedef _Vec3<unsigned int> Point3u;
	typedef _Vec3<float> Vec3;
	typedef _Vec3<double> Vec3d;

	typedef _Vec4<int> Point4;
	typedef _Vec4<unsigned int> Point4u;
	typedef _Vec4<float> Vec4;
	typedef _Vec4<double> Vec4d;

}

#define VC3FLOOR( v ) Point3(cs::floor(v.x), cs::floor(v.y), cs::floor(v.z))
#define VC2FLOOR( v ) Point2(cs::floor(v.x), cs::floor(v.y))

#ifdef CHSL_LINEAR

using cs::Point;
using cs::UPoint;
using cs::Vec2;
using cs::Vec2d;

using cs::Point3;
using cs::UPoint3;
using cs::Vec3;
using cs::Vec3d;

using cs::Point4;
using cs::UPoint4;
using cs::Vec4;
using cs::Vec4d;

#endif