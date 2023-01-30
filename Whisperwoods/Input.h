#pragma once

class Input sealed
{
public:
	Input();
	~Input();

	static Input& Get();

private:
	static Input* s_singleton;
};

