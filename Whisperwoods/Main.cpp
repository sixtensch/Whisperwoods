#include "Core.h"

#include <Windows.h>
#include "Whisperwoods.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

int WINAPI WinMain(
	_In_		HINSTANCE	instance,
	_In_opt_	HINSTANCE	previousInstance,
	_In_		LPSTR		commandLine,
	_In_		int			showCommand)
{
	std::unique_ptr<Whisperwoods> whisperwoods = make_unique<Whisperwoods>();
	whisperwoods->Run();

	return 0;
}