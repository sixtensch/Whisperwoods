#include "core.h"
#include "Config.h"

Config* Config::s_singleton = nullptr;



Config::Config()
{
	if (s_singleton != nullptr)
	{
		throw "Config singleton re-initialization.";	// TODO: Proper exceptions
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
		throw "Config singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}