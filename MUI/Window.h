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
		void SetContent(const std::shared_ptr<UIElement>& element);
		size_t GetHeight();
		size_t GetWidth();
		POINT GetTopLeft();
		RECT GetRect();
		BOOL SetTitle(const wchar_t* title);
		std::wstring GetTitle();
	private:
		HWND m_hWnd;
		HICON m_hIcon;
		HFONT m_hFont;
		std::shared_ptr<UIElement> m_content = NULL;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}