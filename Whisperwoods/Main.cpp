#include "Core.h"

#include <Windows.h>
#include "Whisperwoods.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// Sandbox includes
#include "Input.h";

#include "Keyboard.h";
#include "Mouse.h";

// Callback function of inputs to the window in focus.
LRESULT CALLBACK WindowCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Input& inputRef = Input::Get();

	switch (message)
	{
	case WM_ACTIVATEAPP:
	{
		inputRef.ProcessKeyboardMessage(message, wParam, lParam);
		inputRef.ProcessMouseMessage(message, wParam, lParam);
	}
	break;

	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	{
		inputRef.ProcessMouseMessage(message, wParam, lParam);
	}
	break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		inputRef.ProcessKeyboardMessage(message, wParam, lParam);
	}
	break;

	case WM_SYSKEYDOWN:
	{
		inputRef.ProcessKeyboardMessage(message, wParam, lParam);
	}
	break;

	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(
	_In_		HINSTANCE	instance,
	_In_opt_	HINSTANCE	previousInstance,
	_In_		LPSTR		commandLine,
	_In_		int			showCommand)
{
	std::unique_ptr<Whisperwoods> whisperwoods = make_unique<Whisperwoods>();
	whisperwoods->Run();


	// Sandbox block
	{
		const wchar_t CLASS_NAME[] = L"Sample Window Class";

		WNDCLASS wc = { };

		wc.lpfnWndProc = WindowCallback;
		wc.hInstance = instance;
		wc.lpszClassName = CLASS_NAME;

		RegisterClass(&wc);

		HWND hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			CLASS_NAME,                     // Window class
			L"Learn to Program Windows",    // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window    
			NULL,       // Menu
			instance,  // Instance handle
			NULL        // Additional application data
		);
		
		ShowWindow(hwnd, showCommand);

		Input& inputRef = Input::Get();
		inputRef.BindWindow(hwnd);

		MSG msg = {};
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			KeyboardState keyboardState = inputRef.GetKeyboardState();
			if (keyboardState.IsKeyDown(DirectX::Keyboard::Z))
			{
				int a = 0;
			}

			inputRef.AddKeyToInput(FORWARD, DXKey::W);

			if (inputRef.IsInputDown(FORWARD))
			{
				int a = 0;
			}
		}
		
	}


	return 0;
}