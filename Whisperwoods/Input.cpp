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
	m_inputMap({})
{
	if (s_singleton != nullptr)
	{
		throw "Input singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;
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

void Input::BindWindow(const HWND windowHandle)
{
	m_mouse->SetWindow(windowHandle);
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

void Input::AddKeyToInput(const ABSTRACT_INPUT_ENUM input, const DXKey& key)
{
	m_inputMap[input].Add(key);
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
	const auto dictIterator = m_inputMap.find(input);
	if (dictIterator != m_inputMap.end())
	{
		const cs::List<DXKey> inputKeyList = (*dictIterator).second;

		for (DXKey key : inputKeyList)
		{
			resultBool |= m_currentKeyboardState.IsKeyDown(key);
		}
	}
	
	return resultBool;
}
