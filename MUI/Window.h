#pragma once

#include <Windows.h>
#include <memory>

#include "./UIElement.h"

namespace mui
{
	class Window
	{
	public:
		Window(const wchar_t* title, size_t height = 500, size_t width = 500);
		void Show();
		void SetContent(UIElement* element);
		size_t GetHeight();
		size_t GetWidth();
		POINT GetTopLeft();
		RECT GetRect();
	private:
		HWND m_hWnd;
		HFONT m_hFont;
		std::unique_ptr<UIElement> m_content;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}