#pragma once

class Debug sealed
{
public:
	Debug();
	~Debug();

	static Debug& Get();

private:
	static Debug* s_singleton;
};

