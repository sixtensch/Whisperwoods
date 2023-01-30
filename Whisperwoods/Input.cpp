#include "core.h"
#include "Input.h"

Input* Input::s_singleton = nullptr;

Input::Input()
{
	if (s_singleton != nullptr)
	{
		throw "Input singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
}

Input::~Input()
{
	s_singleton = nullptr;
}

Input& Input::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		throw "Input singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}
