#pragma once

class Renderer sealed
{
public:
	Renderer();
	~Renderer();

	static Renderer& Get();

private:
	static Renderer* s_singleton;
};

