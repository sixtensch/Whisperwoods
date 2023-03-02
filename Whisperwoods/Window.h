#pragma once
#include <Windows.h>

enum CURSOR
{
	CURSOR_DISPLAY_DEFAULT = 0,
	CURSOR_DISPLAY_SHOW    = 1,
	CURSOR_DISPLAY_HIDE    = 2
};

class Window sealed
{
public:
	Window() = delete;
	Window( LPCSTR windowName, HINSTANCE instance, UINT width, UINT height );
	~Window();

	bool PollEvents();

	void Resize( UINT newWidth, UINT newHeight );
	void CursorDisplay(CURSOR showHide);
	void Show(bool show);
	void CloseProgram();

	const HWND& Data() const;
	const UINT GetHeight() const;
	const UINT GetWidth() const;
	const float GetAspectRatio() const;

	static LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	static void LoadPrioritizedWndProc(WNDPROC wndProc);

private:
	static WNDPROC s_externProcedure;

	HWND m_window;

	UINT m_height;
	UINT m_width;
};