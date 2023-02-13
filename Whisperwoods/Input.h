#pragma once
#include <Windows.h>
#include <unordered_map>

// DirectXTK Input headers
#include "Keyboard.h"
#include "Mouse.h"

typedef dx::Keyboard::State KeyboardState;
typedef dx::Mouse::State MouseState;
typedef dx::Keyboard::Keys DXKey;
// TODO: Add DXKey equivalent for mouse so abstract input can be used for mouse as well.

enum Keybind 
{
	KeybindForward = 0,
	KeybindBackward,
	KeybindLeft,
	KeybindRight,	
	KeybindUp,		// Debugging
	KeybindDown,	// Debugging
	KeybindSprint,
	KeybindCrouch,
	KeybindPower,

	KeybindCount	// <-- Keep last
};

class Input sealed
{
public:
	// TODO: Because mouse has to be bound to window to work, should InputInit() take in window handle?
	Input();
	~Input();

	static Input& Get();
	static bool Exists();

	void InputInit(const HWND windowHandle);
	void Update();

	// These functions should only exists inside Windows Callback function.
	void ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

	// Calls to get current state should always be the same for every frame.
	KeyboardState GetKeyboardState() const;
	KeyboardState GetLastKeyboardState() const;
	MouseState GetMouseState() const;
	MouseState GetLastMouseState() const;

	void SetMode(dx::Mouse::Mode mouseMode);

	void AddKeyToInput(const Keybind input, const DXKey key);
	void AddKeysToInput(const Keybind input, const cs::List<DXKey>& keys);

	/* 
		TODO: 
		Add so that variable number of input Enums can be passed to check if all are pressed.
		This can be done for both AND as well as OR.

		Best solution is probably variable number list. 
		This can also be achieved (but probably shouldnt except given a good reason) 
		with bit masks if Enums are set to powers of two.
	*/
	bool IsKeybindDown(Keybind input) const;
	bool IsDXKeyDown(DXKey dxKey) const;
	bool IsKeyPressed(Keybind input) const;
	bool IsDXKeyPressed(DXKey dxKey) const;
	
private:

	void BindWindowToMouse(const HWND windowHandle);
	bool IsKeyBound(const DXKey key);

private:
	static Input* s_singleton;

	KeyboardState m_lastKeyboardState;
	KeyboardState m_currentKeyboardState;
	MouseState m_lastMouseState;
	MouseState m_currentMouseState;

	unique_ptr<dx::Keyboard> m_keyboard;
	unique_ptr<dx::Mouse> m_mouse; // Mouse events wont work if BindWindow() has not been called.
	dx::Keyboard::KeyboardStateTracker m_keyTracker;
	dx::Mouse::ButtonStateTracker m_mouseTracker;


	cs::List<cs::List<DXKey>> m_keybindList;
};

