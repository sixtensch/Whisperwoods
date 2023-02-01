#include "core.h"
#include "Window.h"

Window::Window( LPCWSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow )
{
	//init variables
	m_width = width;
	m_height = height;

	//Init window
	const wchar_t CLASS_NAME[] = L"WindowClassName";

	WNDCLASS wc = { };
	wc.cbClsExtra;
	wc.cbWndExtra;
	wc.hbrBackground;
	wc.hCursor;
	wc.hIcon;
	wc.hInstance = instance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName;
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	RegisterClass( &wc );
	m_window = CreateWindowEx( 0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW,
							   CW_USEDEFAULT, 0, m_width, m_height, 
							   nullptr, nullptr, instance, nullptr );
	
	if ( m_window == nullptr )
	{
		// Window could not be created
		
	}
	ShowWindow( m_window, nCmdShow );
}
Window::~Window()
{}


void Window::Resize(UINT width, UINT height)
{
	


	// Display changes
	m_width = width;
	m_height = height;
}
void Window::CursorDisplay( CURSOR showHide )
{
	switch ( showHide )
	{
		case CURSOR_DISPLAY_DEFAULT:
		case CURSOR_DISPLAY_SHOW:
			ShowCursor( true );
			break;
		case CURSOR_DISPLAY_HIDE:
			ShowCursor( false );
			break;
	}
}


// Returns the HWND of the window
const HWND& Window::Data() const
{
	return m_window;
}
const UINT Window::GetHeight() const
{
	return m_height;
}
const UINT Window::GetWidth() const
{
	return m_width;
}
const float Window::GetAspectRatio() const
{
	return static_cast<float>(m_width) / static_cast<float>(m_height);
}
