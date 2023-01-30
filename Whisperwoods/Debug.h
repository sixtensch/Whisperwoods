#pragma once

class Config sealed
{
public:
	Config();
	~Config();

	static Config& Get();

private:
	static Config* s_singleton;
};

