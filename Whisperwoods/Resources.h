#pragma once

class Resources sealed
{
public:
	Resources();
	~Resources();

	static Resources& Get();

private:
	static Resources* s_singleton;
};

