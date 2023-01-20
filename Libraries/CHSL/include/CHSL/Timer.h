#pragma once

/* CHSL

	|	Timer
	|
	|	- Simple chrono clock wrapper.
	|	- The timer starts at zero, Lap restarts the timer and returns the elapsed time, Peek returns the time but does not reset.

*/









#include <chrono>

namespace cs
{

	class Timer
	{
	private:
		std::chrono::steady_clock::time_point timestamp;

	public:
		Timer();
		float Lap();
		float Peek() const;
	};

}