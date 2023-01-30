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

		static Quaternion GetIdentity();
		static Quaternion GetAxis(const Vec3& axis, float radians);
		static Quaternion GetAxisNormalized(const Vec3& axis, float radians);	// Use when the axis vector is normalized
		static Quaternion GetDeconstruct(const Mat3& matrix);
		static Quaternion GetDeconstruct(const Mat4& matrix);
		static Quaternion GetSlerp(const Quaternion& from, const Quaternion& to, float lambda);

	private:
		Mat4 GetMatrix(float s) const;

	private:
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
					Vec3 m_imaginary;

					struct
					{
						float m_x;
						float m_y;
						float m_z;
					};
				};

				// Real component
				float m_w;
			};

			Vec4 m_vector;
		};
	};

}

#ifdef CHSL_LINEAR
using cs::Quaternion;
#endif
