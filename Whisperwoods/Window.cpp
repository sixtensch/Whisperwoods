#include "core.h"
#include "Window.h"


//Window* Window::s_window = nullptr;


LPARAM WndProc( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_DESTROY: //Cross
		case WM_CLOSE: //Close window
			PostQuitMessage( 0 );
			return 0;

		default:
			return DefWindowProc( window, message, wParam, lParam );
	}
	return 0;
}





Window::Window( LPCSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow )
{
	//if ( s_window != nullptr )
	//{
	//	EXC( "Trying to create a window that already exists" );
	//}
	//s_window = this;

	//init variables
	m_width = width;
	m_height = height;

	//Init window
	const char CLASS_NAME[] = "WindowClassName";

	WNDCLASS wc = {};
	wc.cbClsExtra;
	wc.cbWndExtra;
	wc.hbrBackground;
	wc.hCursor;
	wc.hIcon;
	wc.hInstance = instance;
	wc.lpfnWndProc = WndProc;
	wc.lpszMenuName;
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	RegisterClass( &wc );
	m_window = CreateWindowExA( 0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW,
							   CW_USEDEFAULT, 0, m_width, m_height, 
							   nullptr, nullptr, instance, nullptr );
	
	if ( m_window == nullptr )
	{
		// Window could not be created
		EXC( "Window could not be created in Window::Window(LPCWSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow)" );
		return;
	}
	ShowWindow( m_window, nCmdShow );
}
Window::~Window() {}


void Window::Resize(UINT width, UINT height)
{
	// Resizes the window internally
	int clientLeft = 0;
	int clientTop = 0;
	SetWindowPos( m_window, HWND_TOP, clientLeft, clientTop, width, height, SWP_NOREPOSITION );

	// Display changes
	m_width = width;
	m_height = height;
}
void Window::CursorDisplay( CURSOR showHide )
{
	// TODO: Potential use of GetCursorInfo() to check if counter is -1 or 0


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
