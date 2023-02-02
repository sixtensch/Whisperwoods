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
	m_inputList({})
{
	if (s_singleton != nullptr)
	{
		throw "Input singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		m_inputList.Add({}); // Initializes the container for each input.
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
		throw "Input singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}

void Input::InputInit(const HWND windowHandle)
{
	BindWindowToMouse(windowHandle);

	// Hard coded binding of input values to DXKey values. Read this from a file later?
	AddKeysToInput(FORWARD, { DXKey::W, DXKey::Up });
	AddKeysToInput(BACKWARD, { DXKey::S, DXKey::Down });
	AddKeysToInput(LEFT, { DXKey::A, DXKey::Left });
	AddKeysToInput(RIGHT, { DXKey::D, DXKey::Right });
	AddKeysToInput(SPRINT, { DXKey::LeftShift, DXKey::RightShift });
	AddKeysToInput(CROUCH, { DXKey::LeftControl, DXKey::RightControl });
	AddKeysToInput(POWER, { DXKey::Q, DXKey::NumPad0 });

}

void Input::BindWindowToMouse(const HWND windowHandle)
{
	m_mouse->SetWindow(windowHandle);
}

bool Input::IsKeyBound(const DXKey key)
{
	for (const auto& keyList : m_inputList)
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

void Input::ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_lastKeyboardState = m_keyboard->GetState();
	m_keyboard->ProcessMessage(message, wParam, lParam);
	m_currentKeyboardState = m_keyboard->GetState();
}

void Input::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_lastMouseState = m_mouse->GetState();
	m_mouse->ProcessMessage(message, wParam, lParam);
	m_currentMouseState = m_mouse->GetState();
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

void Input::AddKeyToInput(const ABSTRACT_INPUT_ENUM input, const DXKey key)
{
	if (IsKeyBound(key))
	{
		LOG_ERROR("Key was not bound as it is already bound to other input.");
		return;
	}

	m_inputList[input].Add(key);
}

void Input::AddKeysToInput(const ABSTRACT_INPUT_ENUM input, const cs::List<DXKey>& keys)
{
	for (const DXKey& key : keys)
	{
		AddKeyToInput(input, key);
	}
}

bool Input::IsInputDown(ABSTRACT_INPUT_ENUM input) const
{
	bool resultBool = false;
	const auto& keyList = m_inputList[input];
	for (DXKey key : keyList)
	{
		resultBool |= m_currentKeyboardState.IsKeyDown(key);
	}
	
	return resultBool;
}
