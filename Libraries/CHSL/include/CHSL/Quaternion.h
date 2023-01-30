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
		Quaternion();									// Identity quaternion
		Quaternion(Vec4 components);					// XYZ: imaginary, W: real
		Quaternion(Vec3 imaginary, float real);			// XYZ: imaginary, W: real
		Quaternion(float x, float y, float z, float w);	// XYZ: imaginary, W: real
		Quaternion(const Quaternion&);
		~Quaternion() = default;

		Quaternion Conjugate();
		Quaternion Normal();
		Quaternion Inverse();
		float Norm();
		Mat4 Matrix();					// Matrix representation of the unit quaternion.
		Mat4 MatrixUnrestricted();		// More expensive to calculate, but works for non-unit quaternions.

		Quaternion& ConjugateThis();
		Quaternion& NormalizeThis();
		Quaternion& InvertThis();

		Vec3 operator*(Vec3 vector);			// Apply unit quaternion,					A*B -> transform vector B with unit quaternion A
		Vec4 operator*(Vec4 vector);			// Apply unit quaternion,					A*B -> transform vector B with unit quaternion A
		Quaternion operator*(Quaternion other);	// Quaternion concatenation/multiplication,	A*B -> apply unit quaternion A then B 

		Quaternion operator*(float scalar);		// Complex quaternion scaling. Does not change rotation.
		Quaternion operator+(Quaternion other);	// Complex quaternion addition.

		static Quaternion GetIdentity();
		static Quaternion GetAxis(Vec3 axis, float radians);
		static Quaternion GetDeconstruct(Mat3 matrix);

	private:
		Mat4 GetMatrix(float s);

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
