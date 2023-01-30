#include "core.h"
#include "Sound.h"

#include "core.h"
#include "Input.h"

Sound* Sound::s_singleton = nullptr;



Sound::Sound()
{
	if (s_singleton != nullptr)
	{
		throw "Sound singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
}

Sound::~Sound()
{
	s_singleton = nullptr;
}

Sound& Sound::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		throw "Sound singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}
