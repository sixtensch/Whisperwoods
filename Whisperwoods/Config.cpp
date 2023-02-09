#include "core.h"
#include "Config.h"

Config* Config::s_singleton = nullptr;



Config::Config()
{
	if (s_singleton != nullptr)
	{
		EXC("Config singleton re-initialization.");
	}

	s_singleton = this;
}

Config::~Config()
{
	s_singleton = nullptr;
}

Config& Config::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		EXC("Config singleton not found (is nullptr).");
	}
#endif

	return *s_singleton;
}