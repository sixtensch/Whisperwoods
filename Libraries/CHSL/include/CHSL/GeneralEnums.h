#pragma once

#pragma warning( disable: 26812 )

namespace cs
{

	namespace dimension
	{
		enum D
		{
			First,
			Second,
			Third
		};
	}

	namespace ori
	{

		// Orientations / headings
		enum O : unsigned char
		{
			Up		= 0b100000,
			North	= 0b010000,
			East	= 0b001000,
			South	= 0b000100,
			West	= 0b000010,
			Down	= 0b000001
		};

	}

}