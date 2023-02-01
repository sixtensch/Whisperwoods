#include "core.h"
#include "Input.h"

Input* Input::s_singleton = nullptr;

Input::Input()
	: m_lastKeyboardState({}), 
	m_lastMouseState({}), 
	m_keyboard(make_unique<dx::Keyboard>()), 
	m_mouse(make_unique<dx::Mouse>())
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
}

void Input::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_lastMouseState = m_mouse->GetState();
	m_mouse->ProcessMessage(message, wParam, lParam);
}

KeyboardState Input::GetKeyboardState() const
{
	return m_keyboard->GetState();
}

KeyboardState Input::GetLastKeyboardState() const
{
	return m_lastKeyboardState;
}

MouseState Input::GetMouseState() const
{
	return m_mouse->GetState();
}

MouseState Input::GetLastMouseState() const
{
	return m_lastMouseState;
}
