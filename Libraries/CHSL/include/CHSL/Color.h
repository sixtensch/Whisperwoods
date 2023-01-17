#pragma once

#include "CHSLTypedef.h"
#include "Vector.h"

namespace cs
{

	class Color;

	class ColorA
	{
	public:
		struct BytesA
		{
			byte r, g, b, a;
		};

		float r, g, b, a;

		ColorA();
		ColorA(const Color& c);
		ColorA(const Vec3& v);
		ColorA(const Vec4& v);
		ColorA(float red, float green, float blue, float alpha);
		ColorA(unsigned int colorHex); // 8 byte hexadecimal!

		operator cs::Vec3() const;
		operator cs::Vec4() const;

		BytesA GetBytes() const;
	};
	
	class Color
	{
	public:
		struct Bytes
		{
			byte r, g, b;
		};

		float r, g, b;

		Color();
		Color(float red, float green, float blue);
		Color(const Vec3& v);
		Color(unsigned int colorHex); // 6 byte hexadecimal!

		void Mutate(float ammount, cs::Random& random, bool monochrome = true);

		Bytes GetBytes() const;
		ColorA::BytesA GetBytesA() const;

		operator cs::Vec3() const;

		Color& operator*=(const float& colorM);
	};

}