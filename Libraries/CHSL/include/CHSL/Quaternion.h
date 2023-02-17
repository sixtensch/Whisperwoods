#pragma once

/* CHSL

	|	Quaternion
	|
	|	- Quaternion implementation.

*/









namespace cs
{

	class Quaternion sealed
	{
	public:
		Quaternion();										// Identity quaternion
		Quaternion(const Vec4& components);					// XYZ: imaginary, W: real
		Quaternion(const Vec3& imaginary, float real);		// XYZ: imaginary, W: real
		Quaternion(float x, float y, float z, float w);		// XYZ: imaginary, W: real
		Quaternion(const Quaternion&);
		~Quaternion() = default;

		Quaternion Conjugate() const;
		Quaternion Normal() const;
		Quaternion Inverse() const;
		float Norm() const;
		Mat4 Matrix() const; 				// Matrix representation of the unit quaternion.
		Mat4 MatrixUnrestricted() const; 	// More expensive to calculate, but works for non-unit quaternions.

		Quaternion& ConjugateThis();
		Quaternion& NormalizeThis();
		Quaternion& InvertThis();

		Vec3 operator*(const Vec3& vector) const; 				// Apply unit quaternion,					A*B -> transform vector B with unit quaternion A
		Vec4 operator*(const Vec4& vector) const; 				// Apply unit quaternion,					A*B -> transform vector B with unit quaternion A
		Quaternion operator*(const Quaternion& other) const; 	// Quaternion concatenation/multiplication,	A*B -> apply unit quaternion A then B 

		Quaternion operator*(float scalar) const; 				// Complex quaternion scaling. Does not change rotation.
		Quaternion operator+(const Quaternion& other) const; 	// Complex quaternion addition.

		Quaternion& operator=(const Quaternion& other); 
		Quaternion& operator+=(const Quaternion& other);
		Quaternion& operator*=(const Quaternion& other);

		static Quaternion GetIdentity();
		static Quaternion GetAxis(const Vec3& axis, float radians);
		static Quaternion GetAxisNormalized(const Vec3& axis, float radians);	// Use when the axis vector is normalized
		static Quaternion GetEuler(const Vec3& euler);
		static Quaternion GetEuler(float pitch, float yaw, float roll);
		static Quaternion GetDeconstruct(const Mat3& matrix);
		static Quaternion GetFromTo(const Vec3& from, const Vec3& to);
		static Quaternion GetDirection(const Vec3& direction, Vec3 up = { 0.0f, 1.0f, 0.0f });
		static Quaternion GetDeconstruct(const Mat4& matrix);
		static Quaternion GetSlerp(const Quaternion& from, const Quaternion& to, float lambda);

	private:
		Mat4 GetMatrix(float s) const;

	public:
		// Either:
		// 4D Vector
		// 3D imaginary vector + real component
		// 3 imaginary components + real component

		union
		{
			struct
			{
				// Imaginary components
				union
				{
					Vec3 imaginary;

					struct
					{
						float x;
						float y;
						float z;
					};
				};

				// Real component
				float w;
			};

			Vec4 vector;
		};
	};

}

#ifdef CHSL_LINEAR
using cs::Quaternion;
#endif
