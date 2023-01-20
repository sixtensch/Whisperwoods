#pragma once

/* CHSL

	|	Dimension, Orientation
	|
	|	- Simple utility enums.

*/









#pragma warning( disable: 26812 )

namespace cs
{

	enum Dimension
	{
		DimensionFirst,
		DimensionSecond,
		DimensionThird
	};

	enum Orientation : unsigned char
	{
		OrientationUp		= 0b100000,
		OrientationNorth	= 0b010000,
		OrientationEast		= 0b001000,
		OrientationSouth	= 0b000100,
		OrientationWest		= 0b000010,
		OrientationDown		= 0b000001
	};

}