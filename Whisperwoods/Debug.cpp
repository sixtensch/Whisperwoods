#include "core.h"
#include "Debug.h"

// #include 



Debug* Debug::s_singleton = nullptr;

// extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




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

