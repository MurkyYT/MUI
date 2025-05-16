#pragma once

#include <Windows.h>
#include <memory>

#include "./UIElement.h"
#include "./Events.h"

namespace mui
{
	class Window
	{
	public:
		Window(const wchar_t* title = L"", size_t height = 500, size_t width = 500);
		void Show();
		void Close();
		void SetContent(const std::shared_ptr<UIElement>& element);
		size_t GetHeight();
		size_t GetWidth();
		void SetMaxWidth(size_t width);
		void SetMaxHeight(size_t width);
		void SetMinWidth(size_t width);
		void SetMinHeight(size_t width);
		POINT GetTopLeft();
		RECT GetRect();
		BOOL SetTitle(const wchar_t* title);
		std::wstring GetTitle();

		EventCallback_t KeyDown{ NULL };
		EventCallback_t KeyUp{ NULL };
	private:
		HWND m_hWnd;
		HICON m_hIcon;
		HFONT m_hFont;
		POINT m_minSize = { GetSystemMetrics(SM_CXMINTRACK), GetSystemMetrics(SM_CYMINTRACK) };
		POINT m_maxSize = { GetSystemMetrics(SM_CXMAXTRACK), GetSystemMetrics(SM_CYMAXTRACK) };
		std::shared_ptr<UIElement> m_content = NULL;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}