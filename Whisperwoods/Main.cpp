#include "Core.h"

#include <Windows.h>
#include "Whisperwoods.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

int WINAPI WinMainSafe(
	_In_		HINSTANCE	instance,
	_In_opt_	HINSTANCE	previousInstance,
	_In_		LPSTR		commandLine,
	_In_		int			showCommand)
{
	std::unique_ptr<Whisperwoods> whisperwoods = make_unique<Whisperwoods>();
	whisperwoods->Run();

	return 0;
}

int WINAPI WinMain(
	_In_		HINSTANCE	instance,
	_In_opt_	HINSTANCE	previousInstance,
	_In_		LPSTR		commandLine,
	_In_		int			showCommand)
{
#if CATCH_PROGRAM

	try
	{
		WinMainSafe(instance, previousInstance, commandLine, showCommand);
	}
	catch (const cs::Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

#else

	WinMainSafe(instance, previousInstance, commandLine, showCommand);

#endif

	return 0;
}