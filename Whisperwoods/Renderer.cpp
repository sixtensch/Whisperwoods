#include "core.h"
#include "Renderer.h"

Renderer* Renderer::s_singleton = nullptr;



Renderer::Renderer()
{
	if (s_singleton != nullptr)
	{
		throw "Renderer singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
}

Renderer::~Renderer()
{
	s_singleton = nullptr;
}

Renderer& Renderer::Get()
{
#ifdef WW_Renderer
	if (s_singleton == nullptr)
	{
		throw "Renderer singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}
