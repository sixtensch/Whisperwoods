#pragma once
#include <Windows.h>

// DirectXTK Input headers
#include "Keyboard.h"
#include "Mouse.h"

typedef dx::Keyboard::State KeyboardState;
typedef dx::Mouse::State MouseState;

class Input sealed
{
public:
	Input();

	~Input();

	static Input& Get();
	
	void BindWindow(const HWND windowHandle);

	// These functions should only exists inside Windows Callback function.
	static void ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam);
	static void ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

	KeyboardState GetKeyboardState() const;
	MouseState GetMouseState() const;

private:
	static Input* s_singleton;

	unique_ptr<dx::Keyboard> m_keyboard;
	// Mouse wont work if not bound to window.
	unique_ptr<dx::Mouse> m_mouse;


};

