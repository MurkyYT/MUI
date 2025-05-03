#ifndef MUI_WINDOW_H
#define MUI_WINDOW_H

#include <Windows.h>

namespace mui
{
	class Window
	{
	public:
		Window(const wchar_t* title, size_t height = 500, size_t width = 500);
		void Show();
		size_t GetHeight();
		size_t GetWidth();
		POINT GetTopLeft();
		RECT GetRect();
	private:
		HWND m_hWnd;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

#endif