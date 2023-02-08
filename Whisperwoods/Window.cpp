#include "core.h"
#include "Window.h"
#include "Input.h"


WNDPROC Window::s_externProcedure = nullptr;

LRESULT CALLBACK Window::WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_externProcedure)
	{
		if (s_externProcedure(window, message, wParam, lParam))
		{
			LOG_FRAMETRACE("Extern WndProc override.");
			return true;
		}
	}

	if (Input::Exists())
	{
		static Input& inputRef = Input::Get();

		switch (message)
		{
			case WM_ACTIVATEAPP: // This activates at destruct
			{
				inputRef.ProcessKeyboardMessage(message, wParam, lParam);
				inputRef.ProcessMouseMessage(message, wParam, lParam);
				break;
			}

			case WM_ACTIVATE: // This activates at destruct
			case WM_INPUT:
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:

			case WM_MOUSEHOVER:
			{
				break;
				inputRef.ProcessMouseMessage(message, wParam, lParam);
			}

			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				inputRef.ProcessKeyboardMessage(message, wParam, lParam);
				break;
			}

			case WM_SYSKEYDOWN:
			{
				inputRef.ProcessKeyboardMessage(message, wParam, lParam);
				break;
			}

			default:
				break;
		}
	}

	switch (message)
	{
		case WM_DESTROY: //Cross
		case WM_CLOSE: //Close window
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(window, message, wParam, lParam);
	}

	return 0; // This will never be reached and thats ok.
}



Window::Window( LPCSTR windowName, HINSTANCE instance, UINT width, UINT height )
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
		EXC( "Window could not be created." );
		return;
	}

	ShowWindow( m_window, SW_HIDE );
}

Window::~Window() 
{
}

bool Window::PollEvents()
{
	LOG_FRAMETRACE("Call to PollEvents for Window.");

	static MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Return optional int containing exit code if the message is a quit message
		if (msg.message == WM_QUIT)
		{
			LOG_FRAMETRACE("ProcessMessages caught WM_QUIT message.");
			return true;
		}

		// Create auxillary messages and then pass to WndProc
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return false;
}



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

void Window::Show(bool show)
{
	if (m_window == nullptr)
	{
		return;
	}

	ShowWindow(m_window, show ? SW_SHOW : SW_HIDE);
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

void Window::LoadPrioritizedWndProc(WNDPROC wndProc)
{
	s_externProcedure = wndProc;
}
