#pragma once
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
	Window( LPCWSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow );
	~Window();

	static LPARAM CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);


	void Resize( UINT newWidth, UINT newHeight );
	void CursorDisplay(CURSOR showHide);


	const HWND& Data() const;
	const UINT GetHeight() const;
	const UINT GetWidth() const;
	const float GetAspectRatio() const;

private:
public:
private:
	HWND m_window;

	UINT m_height;
	UINT m_width;
};