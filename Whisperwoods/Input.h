#pragma once
#include <Windows.h>
#include <unordered_map>

// DirectXTK Input headers
#include "Keyboard.h"
#include "Mouse.h"

typedef dx::Keyboard::State KeyboardState;
typedef dx::Mouse::State MouseState;
typedef dx::Keyboard::Keys DXKey;

/*
Dictionary with key containing the abstract key as an enum like ENUM_FORWARD

Value is a list of keyboard state for specific keys that are dynamically added [keyboardstate.A, keyboardstate.Del, ...].

When Input is asked .GetInput(ENUM_FORWARD), it goes in the dictionary for the enum and ORs the array.

result = false;
foreach bool key in list:
	result |= key;

return result;

*/

enum ABSTRACT_INPUT_ENUM {

	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT,
	SPRINT,
	CROUCH,
	POWER,
	INPUT_COUNT // This has to always be at the end of the enum.
};

class Input sealed
{
public:
	Input();

	~Input();

	static Input& Get();
	
	void BindWindow(const HWND windowHandle);

	// These functions should only exists inside Windows Callback function.
	void ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

	// Calls to get current state should always be the same for every frame.
	KeyboardState GetKeyboardState() const;
	KeyboardState GetLastKeyboardState() const;
	MouseState GetMouseState() const;
	MouseState GetLastMouseState() const;

	void AddKeyToInput(const ABSTRACT_INPUT_ENUM input, const DXKey& key);
	void AddKeysToInput(const ABSTRACT_INPUT_ENUM input, const cs::List<DXKey>& keys);
	bool IsInputDown(ABSTRACT_INPUT_ENUM input) const;

private:
	static Input* s_singleton;

	KeyboardState m_lastKeyboardState;
	KeyboardState m_currentKeyboardState;
	MouseState m_lastMouseState;
	MouseState m_currentMouseState;

	unique_ptr<dx::Keyboard> m_keyboard;
	// Mouse wont work if not bound to window.
	unique_ptr<dx::Mouse> m_mouse;

	std::unordered_map<ABSTRACT_INPUT_ENUM, cs::List<DXKey>> m_inputMap;
};

