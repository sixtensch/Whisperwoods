#include "core.h"
#include "Resources.h"

Resources* Resources::s_singleton = nullptr;



Resources::Resources()
{
	if (s_singleton != nullptr)
	{
		throw "Resources singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
}

Resources::~Resources()
{
	s_singleton = nullptr;
}

Resources& Resources::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		throw "Resources singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}