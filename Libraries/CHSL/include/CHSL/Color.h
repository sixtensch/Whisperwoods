#pragma once

/* CHSL

	|	Color3, Color4, Color3f, Color4f
	|
	|	- Basic Color classes.
	|	- Color3 and Color4 implement byte components, while their counterparts implement float components.
	|	- Color3(f) implements RGB, while Color4(f) implements RGBA.

*/







#include "CHSLTypedef.h"
#include "Vector.h"

namespace cs
{

	class Color3;
	class Color4;
	class Color3f;
	class Color4f;

	class Color3
	{
	public:
		unsigned char r, g, b;

	public:
		Color3();
		Color3(unsigned char red, unsigned char green, unsigned char blue);
		Color3(const Point3& v);
		Color3(unsigned int colorHex); // 6 byte hexadecimal

		operator cs::Point3() const;
		operator cs::Point4() const;
		operator Color4() const;
		operator Color3f() const;
		operator Color4f() const;
	};

	class Color4
	{
	public:
		unsigned char r, g, b, a;

	public:
		Color4();
		Color4(const Color3& c);
		Color4(const Point3& v, unsigned char a = 255);
		Color4(const Point4& v);
		Color4(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
		Color4(unsigned int colorHex); // 8 byte hexadecimal

		operator cs::Point3() const;
		operator cs::Point4() const;
		operator Color3() const;
		operator Color3f() const;
		operator Color4f() const;
	};



	class Color3f
	{
	public:
		float r, g, b;

	public:
		Color3f();
		Color3f(float red, float green, float blue);
		Color3f(const Vec3& v);
		Color3f(unsigned int colorHex); // 6 byte hexadecimal!

		void Mutate(float ammount, cs::Random& random, bool monochrome = true);

		operator cs::Vec3() const;
		operator cs::Vec4() const;
		operator Color3() const;
		operator Color4() const;
		operator Color4f() const;

		Color3f& operator*=(const float& modifier); // Scales RGB components by modifier
	};

	class Color4f
	{
	public:
		float r, g, b, a;

	public:
		Color4f();
		Color4f(const Color3f& c);
		Color4f(const Vec3& v);
		Color4f(const Vec4& v);
		Color4f(float red, float green, float blue, float alpha);
		Color4f(unsigned int colorHex); // 8 byte hexadecimal!

		void Mutate(float ammount, cs::Random& random, bool monochrome = false);

		operator cs::Vec3() const;
		operator cs::Vec4() const;
		operator Color3() const;
		operator Color4() const;
		operator Color3f() const;

		Color4f& operator*=(const float& modifier); // Scales RGB components by modifier
	};

}