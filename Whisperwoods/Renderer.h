#pragma once

class Renderer sealed
{
public:
	Renderer();
	~Renderer();

private:
	static Renderer* s_singleton;
};

