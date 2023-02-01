#include "core.h"
#include "Input.h"

Input* Input::s_singleton = nullptr;

Input::Input()
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
	m_keyboard = make_unique<dx::Keyboard>();
	m_mouse = make_unique<dx::Mouse>();

	m_mouse->SetWindow(windowHandle);
}

void Input::ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	dx::Keyboard::ProcessMessage(message, wParam, lParam);
}

void Input::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	dx::Mouse::ProcessMessage(message, wParam, lParam);
}

KeyboardState Input::GetKeyboardState() const
{
	return m_keyboard->GetState();
}

MouseState Input::GetMouseState() const
{
	return m_mouse->GetState();
}
