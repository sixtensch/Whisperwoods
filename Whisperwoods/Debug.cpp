#include "core.h"
#include "Debug.h"

Debug* Debug::s_singleton = nullptr;



Debug::Debug()
{
	if (s_singleton != nullptr)
	{
		throw "Debug singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
}

Debug::~Debug()
{
	s_singleton = nullptr;
}

Debug& Debug::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		throw "Debug singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}
