#pragma once

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