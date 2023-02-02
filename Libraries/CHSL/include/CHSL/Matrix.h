#pragma once

/* CHSL

	|	Matrix<T>
	|	Matrix2<T>	(Mat2, Mat2i, Mat2d)
	|	Matrix3<T>	(Mat3, Mat3i, Mat3d)
	|	Matrix4<T>	(Mat4, Mat4i, Mat4d)
	|
	|	- Mathematical matrix implementation, with additional functionality for 2x2, 3x3, and 4x4 matrices.
	|	- Functions for generating certain pre-defined matrices.
	|	- SIMD implementation.

*/









#include <stdarg.h>
#include <initializer_list>
#include <immintrin.h>

#include "Vector.h"
#include "Math.h"

namespace cs
{

	// All matrices are row-major
	// They transform column vectors in a left-oriented space
	// Matrix multiplication is resolved in a right-to-left order

	template<typename T, int W, int H = W>
	class Matrix
	{
	public:
		Matrix();
		Matrix(T content...);
		Matrix(std::initializer_list<T> content);
		Matrix(const Matrix&);

		template<typename T2>
		operator Matrix<T2, W, H>();

		const T& operator()(int x, int y) const;
		T& operator()(int x, int y);

		bool operator==(const Matrix& other) const;
		bool operator!=(const Matrix& other) const;
		
		template<int W2>
		Matrix<T, W2, H> operator*(const Matrix<T, W2, W>& other) const;
		Matrix operator+(const Matrix& other) const;
		Matrix operator-(const Matrix& other) const;
		Matrix operator*(const T& scalar) const;

		Matrix& operator=(const Matrix & other);
		Matrix& operator*=(const Matrix& other);
		Matrix& operator+=(const Matrix& other);
		Matrix& operator-=(const Matrix& other);
		Matrix& operator*=(const T& scalar);

		const T& Get(int x, int y) const;
		T& Get(int x, int y);

		Matrix<T, H, W> Transpose() const;

		const T* Data() const;
		T* Data();

	public:
		T data[H * W];
	};

	template<typename T>
	class alignas(sizeof(T)) Matrix2 : public Matrix<T, 2>
	{
	public:
		Matrix2();
		Matrix2(T a1, T a2, T b1, T b2);
		Matrix2(std::initializer_list<T> content);
		Matrix2(const Matrix<T, 2>&);
		Matrix2(const Matrix2&);

		_Vec2<T> operator*(const _Vec2<T>& vector) const;
		Matrix2 operator*(const Matrix2& matrix) const;

		T Trace() const;
		T Determinant() const;
		Matrix2 Inverse() const;
	};

	template<typename T>
	class Matrix4;

	template<typename T>
	class alignas(sizeof(T)) Matrix3 : public Matrix<T, 3>
	{
	public:
		Matrix3();
		Matrix3(T a1, T a2, T a3, T b1, T b2, T b3, T c1, T c2, T c3);
		Matrix3(std::initializer_list<T> content);
		Matrix3(const Matrix<T, 3>&);
		Matrix3(const Matrix3&);

		_Vec2<T> operator*(const _Vec2<T>& vector) const;
		_Vec3<T> operator*(const _Vec3<T>& vector) const;
		Matrix3 operator*(const Matrix3& matrix) const;
		Matrix4<T> operator*(const Matrix4<T>& matrix) const;

		T Trace() const;
		T Determinant() const;
		Matrix3 Inverse() const;
	};

	template<typename T>
	class alignas(sizeof(T)) Matrix4 : public Matrix<T, 4>
	{
	public:
		Matrix4();
		Matrix4(T a1, T a2, T a3, T a4, T b1, T b2, T b3, T b4, T c1, T c2, T c3, T c4, T d1, T d2, T d3, T d4);
		Matrix4(std::initializer_list<T> content);
		Matrix4(const Matrix<T, 4>&);
		Matrix4(const Matrix3<T>&);
		Matrix4(const Matrix4&);
#ifdef CHSL_DX
		Matrix4(DirectX::XMMATRIX xmmatrix);

		DirectX::XMMATRIX XMMatrix() const;
#endif

		Matrix4 operator*(const Matrix3<T>& matrix) const;
		Matrix4 operator*(const Matrix4& matrix) const;

		_Vec3<T> operator*(const _Vec3<T>& vector) const;
		_Vec4<T> operator*(const _Vec4<T>& vector) const;

		T Trace() const;
	};



	// Typedefs

	typedef Matrix2<float>	Mat2;
	typedef Matrix2<double>	Mat2d;
	typedef Matrix2<int>	Mat2i;

	typedef Matrix3<float>	Mat3;
	typedef Matrix3<double>	Mat3d;
	typedef Matrix3<int>	Mat3i;

	typedef Matrix4<float>	Mat4;
	typedef Matrix4<double>	Mat4d;
	typedef Matrix4<int>	Mat4i;



	namespace Mat
	{

		// 2D

		Mat2 rotation2(float radians);

		Mat2 scale2(float scale);
		Mat2 scale2(float xScale, float yScale);
		Mat2 scale2(const Vec2& scale);

		Mat3 translation2(const Vec2& translation);
		Mat3 translation2(float x, float y);

		Vec2 transform(const Mat2& matrix, const Vec2& vector);
		Vec2 transform(const Mat3& matrix, const Vec2& vector);

		// 3D

		Mat3 rotation3(float xR, float yR, float zR);
		Mat3 rotation3X(float radians);
		Mat3 rotation3Y(float radians);
		Mat3 rotation3Z(float radians);

		Mat3 scale3(float scale);
		Mat3 scale3(float xScale, float yScale, float zScale);
		Mat3 scale3(const Vec3& scale);

		Mat4 translation3(const Vec3& translation);
		Mat4 translation3(float x, float y, float z);

		Vec3 transform(const Mat3& matrix, const Vec3& vector);
		Vec3 transform(const Mat4& matrix, const Vec3& vector);

	}



	// Definitions

	template<typename T, int W, int H>
	inline Matrix<T, W, H>::Matrix()
		:
		data()
	{
	}

	template<typename T, int W, int H>
	Matrix<T, W, H>::Matrix(T content...)
		:
		Matrix()
	{
		va_list l;
		va_start(l, content);

		data[0] = content;

		for (int i = 1; i < W * H; i++)
		{
			data[i] = va_arg(l, T);
		}

		va_end(l);
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>::Matrix(std::initializer_list<T> content)
		:
		Matrix()
	{
		int i = 0;
		for (auto iter = content.begin(); iter != content.end() && i < W * H; iter++)
		{
			data[i] = *iter;
			i++;
		}
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>::Matrix(const Matrix& lVal)
		:
		Matrix()
	{
		for (int i = 0; i < W * H; i++)
		{
			data[i] = lVal.data[i];
		}
	}

	template<typename T, int W, int H>
	inline const T& Matrix<T, W, H>::operator()(int x, int y) const
	{
		return data[x + y * W];
	}

	template<typename T, int W, int H>
	inline T& Matrix<T, W, H>::operator()(int x, int y)
	{
		return data[x + y * W];
	}

	template<typename T, int W, int H>
	inline bool Matrix<T, W, H>::operator==(const Matrix& other) const
	{
		for (int i = 0; i < W * H; i++)
		{
			if (data[i] != other.data[i])
			{
				return false;
			}
		}

		return true;
	}

	template<typename T, int W, int H>
	inline bool Matrix<T, W, H>::operator!=(const Matrix& other) const
	{
		return !(*this == other);
	}

	//template<typename T, int W, int H>
	//Matrix<T, W, H> Matrix<T, W, H>::operator*(const Matrix& other) const
	//{
	//	Matrix m;

	//	for (int i = 0; i < W * H; i++)
	//	{
	//		m.data[i] = data[i] * other.data[i];
	//	}

	//	return m;
	//}

	template<typename T, int W, int H>
	inline Matrix<T, W, H> Matrix<T, W, H>::operator+(const Matrix& other) const
	{
		Matrix m;

		for (int i = 0; i < W * H; i++)
		{
			m.data[i] = data[i] + other.data[i];
		}

		return m;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H> Matrix<T, W, H>::operator-(const Matrix& other) const
	{
		Matrix m;

		for (int i = 0; i < W * H; i++)
		{
			m.data[i] = data[i] - other.data[i];
		}

		return m;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H> Matrix<T, W, H>::operator*(const T& scalar) const
	{
		Matrix m;

		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				m.data[x + y * W] = data[x + y * W] * scalar;
			}
		}

		return m;
	}

	//template<typename T, int W, int H>
	//Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix& other)
	//{
	//	// TODO: insert return statement here
	//}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix& other)
	{
		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				data[x + y * W] += other.data[x + y * W];
			}
		}

		return *this;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix& other)
	{
		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				data[x + y * W] -= other.data[x + y * W];
			}
		}

		return *this;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const T& scalar)
	{
		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				data[x + y * W] *= scalar;
			}
		}

		return *this;
	}

	template<typename T, int W, int H>
	inline const T& Matrix<T, W, H>::Get(int x, int y) const
	{
		return data[x + y * W];
	}

	template<typename T, int W, int H>
	inline T& Matrix<T, W, H>::Get(int x, int y)
	{
		return data[x + y * W];
	}

	template<typename T, int W, int H>
	Matrix<T, H, W> Matrix<T, W, H>::Transpose() const
	{
		Matrix<T, H, W> m;

		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				m.data[y + x * W] = data[x + y * W];
			}
		}

		return m;
	}

	template<typename T, int W, int H>
	inline const T* Matrix<T, W, H>::Data() const
	{
		return data;
	}

	template<typename T, int W, int H>
	inline T* Matrix<T, W, H>::Data()
	{
		return data;
	}

	template<typename T, int W, int H>
	template<typename T2>
	Matrix<T, W, H>::operator Matrix<T2, W, H>()
	{
		Matrix<T2, W, H> m;

		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				m.data[x + y * W] = (T2)(data[x + y * W]);
			}
		}

		return m;
	}

	template<typename T, int W, int H>
	template<int W2>
	Matrix<T, W2, H> Matrix<T, W, H>::operator*(const Matrix<T, W2, W>& other) const
	{
		Matrix<T, W2, H> m;

		for (int x = 0; x < W2; x++)
		{
			for (int y = 0; y < H; y++)
			{
				T& current = m.data[x + y * W2];

				for (int i = 0; i < W; i++)
				{
					current += data[i + y * W] * other.data[x + i * W];
				}
			}
		}

		return m;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>& Matrix<T, W, H>::operator=(const Matrix& other)
	{
		for (int i = 0; i < W * H; i++)
		{
			data[i] = other.data[i];
		}

		return *this;
	}

	template<typename T, int W, int H>
	inline Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix<T, W, H>& other)
	{
		return *this = (*this * other);
	}



	// Matrix2

	template<typename T>
	inline Matrix2<T>::Matrix2()
		:
		Matrix<T, 2>()
	{
	}

	template<typename T>
	inline Matrix2<T>::Matrix2(T a1, T a2, T b1, T b2)
		:
		Matrix<T, 2> { a1, a2, b1, b2 }
	{
	}

	template<typename T>
	inline Matrix2<T>::Matrix2(std::initializer_list<T> content)
		:
		Matrix<T, 2>(content)
	{
	}

	template<typename T>
	inline Matrix2<T>::Matrix2(const Matrix<T, 2>& lVal)
		:
		Matrix<T, 2>(lVal)
	{
	}

	template<typename T>
	inline Matrix2<T>::Matrix2(const Matrix2& lVal)
		:
		Matrix<T, 2>(lVal)
	{
	}

	template<typename T>
	inline _Vec2<T> Matrix2<T>::operator*(const _Vec2<T>& v) const
	{
		return _Vec2<T>(
			v.x * (*this)(0, 0) + v.y * (*this)(1, 0) + (*this)(2, 0),
			v.x * (*this)(0, 1) + v.y * (*this)(1, 1) + (*this)(2, 1)
		);
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator*(const Matrix2& matrix) const
	{
		return (Matrix<T, 2>)(*this) * matrix;
	}

	template<typename T>
	inline T Matrix2<T>::Trace() const
	{
		return (*this)(0, 0) + (*this)(1, 1);
	}

	template<typename T>
	inline T Matrix2<T>::Determinant() const
	{
		return (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::Inverse() const
	{
		T d = ((T)1) / this->Determinant();

		return Matrix2
		{
			(*this)(1, 1) * d, -(*this)(1, 0) * d,
			-(*this)(0, 1) * d, (*this)(0, 0) * d
		};
	}



	// Matrix3

	template<typename T>
	inline Matrix3<T>::Matrix3()
		:
		Matrix<T, 3>{ 1, 0, 0, 0, 1, 0, 0, 0, 1 }
	{
	}

	template<typename T>
	inline Matrix3<T>::Matrix3(T a1, T a2, T a3, T b1, T b2, T b3, T c1, T c2, T c3)
		:
		Matrix<T, 3>{ a1, a2, a3, b1, b2, b3, c1, c2, c3 }
	{
	}

	template<typename T>
	inline Matrix3<T>::Matrix3(std::initializer_list<T> content)
		:
		Matrix<T, 3>(content)
	{
	}

	template<typename T>
	inline Matrix3<T>::Matrix3(const Matrix<T, 3>& lVal)
		:
		Matrix<T, 3>(lVal)
	{
	}

	template<typename T>
	inline Matrix3<T>::Matrix3(const Matrix3& lVal)
		:
		Matrix<T, 3>(lVal)
	{
	}

	template<typename T>
	inline _Vec2<T> Matrix3<T>::operator*(const _Vec2<T>& v) const
	{
		return _Vec2<T>(
			v.x * (*this)(0, 0) + v.y * (*this)(1, 0) + (*this)(2, 0),
			v.x * (*this)(0, 1) + v.y * (*this)(1, 1) + (*this)(2, 1)
		);
	}

	template<typename T>
	inline _Vec3<T> Matrix3<T>::operator*(const _Vec3<T>& v) const
	{
		return _Vec3<T>(
			v.x * (*this)(0, 0) + v.y * (*this)(1, 0) + v.z * (*this)(2, 0),
			v.x * (*this)(0, 1) + v.y * (*this)(1, 1) + v.z * (*this)(2, 1),
			v.x * (*this)(0, 2) + v.y * (*this)(1, 2) + v.z * (*this)(2, 2)
		);
	}

	template<>
	inline Matrix3<float> Matrix3<float>::operator*(const Matrix3& matrix) const
	{
		Matrix3 product;

		const __m128i storeMask = _mm_set_epi32(0, -1, -1, -1);

		const __m128 otherRow0 = _mm_load_ps(&(matrix.data[0]));
		const __m128 otherRow1 = _mm_load_ps(&(matrix.data[3]));
		const __m128 otherRow2 = _mm_load_ps(&(matrix.data[6]));

		for (int i = 0; i < 3; i++)
		{
			__m128 thisCurrentRowAllX = _mm_set1_ps(data[i * 3 + 0]);
			__m128 thisCurrentRowAllY = _mm_set1_ps(data[i * 3 + 1]);
			__m128 thisCurrentRowAllZ = _mm_set1_ps(data[i * 3 + 2]);

			__m128 newRowXComponents = _mm_mul_ps(thisCurrentRowAllX, otherRow0);
			__m128 newRowYComponents = _mm_mul_ps(thisCurrentRowAllY, otherRow1);
			__m128 newRowZComponents = _mm_mul_ps(thisCurrentRowAllZ, otherRow2);

			__m128 newRow = _mm_add_ps(_mm_add_ps(newRowXComponents, newRowYComponents), newRowZComponents);

			_mm_maskstore_ps(&(product.data[i * 3]), storeMask, newRow);
		}

		return product;
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator*(const Matrix3& matrix) const
	{
		return (Matrix<T, 3>)(*this) * matrix;
	}

	template<typename T>
	inline Matrix4<T> Matrix3<T>::operator*(const Matrix4<T>& matrix) const
	{
		return Matrix4<T>(*this) * matrix;
	}

	template<typename T>
	inline T Matrix3<T>::Trace() const
	{
		return (*this)(0, 0) + (*this)(1, 1) + (*this)(2, 2);
	}

	template<typename T>
	inline T Matrix3<T>::Determinant() const
	{
		return
			(*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2) +
			(*this)(1, 0) * (*this)(2, 1) * (*this)(0, 2) +
			(*this)(2, 0) * (*this)(0, 1) * (*this)(1, 2) -
			(*this)(0, 0) * (*this)(2, 1) * (*this)(1, 2) -
			(*this)(1, 0) * (*this)(0, 1) * (*this)(2, 2) -
			(*this)(2, 0) * (*this)(1, 1) * (*this)(0, 2);
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::Inverse() const
	{
		Matrix3 m = *this;

		for (int x = 0; x < 3; x++)
		{
			int x1 = imod(x + 1, 3);
			int x2 = imod(x + 2, 3);

			for (int y = 0; y < 3; y++)
			{
				int y1 = imod(y + 1, 3);
				int y2 = imod(y + 2, 3);

				m.data[x + y * 3] =
					(*this)(x1, y1) * (*this)(x2, y2) -
					(*this)(x1, y2) * (*this)(x2, y1);
			}
		}

		T d = ((T)1) / (
			(*this)(0, 0) * m(0, 0) +
			(*this)(1, 0) * m(1, 0) +
			(*this)(2, 0) * m(2, 0));

		return m.Transpose() * d;
	}

	template<typename T>
	inline Matrix4<T>::Matrix4()
		:
		Matrix<T, 4>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }
	{
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(T a1, T a2, T a3, T a4, T b1, T b2, T b3, T b4, T c1, T c2, T c3, T c4, T d1, T d2, T d3, T d4)
		:
		Matrix<T, 4>{ a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4 }
	{
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(std::initializer_list<T> content)
		:
		Matrix<T, 4>(content)
	{
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(const Matrix<T, 4>& lVal)
		:
		Matrix<T, 4>(lVal)
	{
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(const Matrix3<T>& lVal)
		:
		Matrix4
		{
			lVal(0, 0), lVal(1, 0), lVal(2, 0), 0,
			lVal(0, 1), lVal(1, 1), lVal(2, 1), 0,
			lVal(0, 2), lVal(1, 2), lVal(2, 2), 0,
			0,			0,			0,			1
		}
	{
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(const Matrix4& lVal)
		:
		Matrix<T, 4>(lVal)
	{
	}

#ifdef CHSL_DX

	template<>
	inline Matrix4<float>::Matrix4(DirectX::XMMATRIX xmmatrix)
		:
		Matrix<float, 4>()
	{
		DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&data, xmmatrix);
	}

	template<typename T>
	inline Matrix4<T>::Matrix4(DirectX::XMMATRIX xmmatrix)
	{
		// What?
	}

	template<>
	inline DirectX::XMMATRIX Matrix4<float>::XMMatrix() const
	{
		return DirectX::XMMATRIX(data);
	}

	template<typename T>
	inline DirectX::XMMATRIX Matrix4<T>::XMMatrix() const
	{
		return DirectX::XMMATRIX();
	}

#endif

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator*(const Matrix3<T>& matrix) const
	{
		return *this * Matrix4(matrix);
	}

	template<>
	inline Matrix4<float> Matrix4<float>::operator*(const Matrix4& matrix) const
	{
		Matrix4 product;
		
		const __m128 otherRow0 = _mm_load_ps(&(matrix.data[0]));
		const __m128 otherRow1 = _mm_load_ps(&(matrix.data[4]));
		const __m128 otherRow2 = _mm_load_ps(&(matrix.data[8]));
		const __m128 otherRow3 = _mm_load_ps(&(matrix.data[12]));

		for (int i = 0; i < 4; i++)
		{
			__m128 thisCurrentRowAllX = _mm_set1_ps(data[i * 4 + 0]);
			__m128 thisCurrentRowAllY = _mm_set1_ps(data[i * 4 + 1]);
			__m128 thisCurrentRowAllZ = _mm_set1_ps(data[i * 4 + 2]);
			__m128 thisCurrentRowAllW = _mm_set1_ps(data[i * 4 + 3]);

			__m128 newRowXComponents = _mm_mul_ps(thisCurrentRowAllX, otherRow0);
			__m128 newRowYComponents = _mm_mul_ps(thisCurrentRowAllY, otherRow1);
			__m128 newRowZComponents = _mm_mul_ps(thisCurrentRowAllZ, otherRow2);
			__m128 newRowWComponents = _mm_mul_ps(thisCurrentRowAllW, otherRow3);

			__m128 newRow = 
				_mm_add_ps(_mm_add_ps(_mm_add_ps(
					newRowXComponents, newRowYComponents), newRowZComponents), newRowWComponents);

			_mm_store_ps(&(product.data[i * 4]), newRow);
		}

		return product;
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator*(const Matrix4& matrix) const
	{
		return (Matrix<T, 4>)(*this) * matrix;
	}

	template<typename T>
	inline _Vec3<T> Matrix4<T>::operator*(const _Vec3<T>& v) const
	{
		return Vec3(
			v.x * (*this)(0, 0) + v.y * (*this)(1, 0) + v.z * (*this)(2, 0) + (*this)(3, 0),
			v.x * (*this)(0, 1) + v.y * (*this)(1, 1) + v.z * (*this)(2, 1) + (*this)(3, 1),
			v.x * (*this)(0, 2) + v.y * (*this)(1, 2) + v.z * (*this)(2, 2) + (*this)(3, 2)
		);
	}

	template<typename T>
	inline _Vec4<T> Matrix4<T>::operator*(const _Vec4<T>& v) const
	{
		return _Vec4<T>(
			v.x * (*this)(0, 0) + v.y * (*this)(1, 0) + v.z * (*this)(2, 0) + v.w + (*this)(3, 0),
			v.x * (*this)(0, 1) + v.y * (*this)(1, 1) + v.z * (*this)(2, 1) + v.w + (*this)(3, 1), 
			v.x * (*this)(0, 2) + v.y * (*this)(1, 2) + v.z * (*this)(2, 2) + v.w + (*this)(3, 2),
			v.x * (*this)(0, 3) + v.y * (*this)(1, 3) + v.z * (*this)(2, 3) + v.w + (*this)(3, 3)
		);
	}

	template<typename T>
	inline T Matrix4<T>::Trace() const
	{
		return (*this)(0, 0) + (*this)(1, 1) + (*this)(2, 2) + (*this)(3, 3);
	}

}

#ifdef CHSL_LINEAR

namespace Mat = cs::Mat;

using cs::Matrix;
using cs::Matrix2;
using cs::Matrix3;
using cs::Matrix4;

using cs::Mat2;
using cs::Mat2d;
using cs::Mat2i;

using cs::Mat3;
using cs::Mat3d;
using cs::Mat3i;

using cs::Mat4;
using cs::Mat4d;
using cs::Mat4i;

#endif