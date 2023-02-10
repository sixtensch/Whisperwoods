#include "core.h"
#include "Input.h"

Input* Input::s_singleton = nullptr;

Input::Input()
	: m_lastKeyboardState({}),
	m_currentKeyboardState({}),
	m_lastMouseState({}),
	m_currentMouseState({}),
	m_keyboard(make_unique<dx::Keyboard>()),
	m_mouse(make_unique<dx::Mouse>()),
	m_keybindList({})
{
	if (s_singleton != nullptr)
	{
		EXC("Input singleton re-initialization.");
	}

	s_singleton = this;
	for (int i = 0; i < KeybindCount; i++)
	{
		m_keybindList.Add({}); // Initializes the container for each input.
	}
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
		EXC("Input singleton not found (is nullptr).");
	}
#endif

	return *s_singleton;
}

bool Input::Exists()
{
	return s_singleton != nullptr;
}

void Input::InputInit(const HWND windowHandle)
{
	BindWindowToMouse(windowHandle);

	// Hard coded binding of input values to DXKey values. Read this from a file later?
	AddKeysToInput(KeybindForward, { DXKey::W, DXKey::Up });
	AddKeysToInput(KeybindBackward, { DXKey::S, DXKey::Down });
	AddKeysToInput(KeybindLeft, { DXKey::A, DXKey::Left });
	AddKeysToInput(KeybindRight, { DXKey::D, DXKey::Right });
	AddKeysToInput(KeybindUp, { DXKey::Space });
	AddKeysToInput(KeybindDown, { DXKey::LeftControl });
	AddKeysToInput(KeybindSprint, { DXKey::LeftShift, DXKey::RightShift });
	AddKeysToInput(KeybindCrouch, { DXKey::LeftControl, DXKey::RightControl });
	AddKeysToInput(KeybindPower, { DXKey::Q, DXKey::NumPad0 });

}

void Input::BindWindowToMouse(const HWND windowHandle)
{
	m_mouse->SetWindow(windowHandle);
}

bool Input::IsKeyBound(const DXKey key)
{
	// TODO: Implement linear search check for const references when available CHSL. (Just for readability)
	for (const auto& keyList : m_keybindList)
	{
		for (DXKey boundKey : keyList)
		{
			if (boundKey == key)
			{
				return true;
			}
		}
	}

	return false;
}

void Input::Update()
{
	m_lastKeyboardState = m_currentKeyboardState;
	m_lastMouseState = m_currentMouseState;

	m_currentKeyboardState = m_keyboard->GetState();
	m_currentMouseState = m_mouse->GetState();
}

void Input::ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_singleton == nullptr)
		return;

	m_keyboard->ProcessMessage(message, wParam, lParam);
}

void Input::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_singleton == nullptr)
		return;

	m_mouse->ProcessMessage(message, wParam, lParam);
}

KeyboardState Input::GetKeyboardState() const
{
	return m_currentKeyboardState;
}

KeyboardState Input::GetLastKeyboardState() const
{
	return m_lastKeyboardState;
}

MouseState Input::GetMouseState() const
{
	return m_currentMouseState;
}

MouseState Input::GetLastMouseState() const
{
	return m_lastMouseState;
}

void Input::AddKeyToInput(const Keybind input, const DXKey key)
{
	if (IsKeyBound(key))
	{
		LOG_ERROR("Key failed to bind as it is already bound to other input.");
		return;
	}

	m_keybindList[input].Add(key);
}

void Input::AddKeysToInput(const Keybind input, const cs::List<DXKey>& keys)
{
	for (const DXKey& key : keys)
	{
		AddKeyToInput(input, key);
	}
}

bool Input::IsKeybindDown(Keybind input) const
{
	bool resultBool = false;
	const auto& keyList = m_keybindList[input];
	for (DXKey key : keyList)
	{
		resultBool |= m_currentKeyboardState.IsKeyDown(key);
	}
	
	return resultBool;
}
