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
		void SetWidth(size_t width);
		void SetHeight(size_t width);
		BOOL SetCaptionColor(COLORREF color);
		size_t GetHeight();
		size_t GetWidth();
		void Activate() { SetForegroundWindow(m_hWnd); SetFocus(m_hWnd); }
		void SetMaxWidth(size_t width);
		void SetMaxHeight(size_t width);
		void SetMinWidth(size_t width);
		void SetMinHeight(size_t width);
		POINT GetTopLeft();
		RECT GetRect();
		HWND GetHWND() { return m_hWnd; }
		BOOL IsHidden() { return !IsWindowVisible(m_hWnd); }
		BOOL SetTitle(const wchar_t* title);
		std::wstring GetTitle();

		void SetBackgroundColor(COLORREF color) { m_backgroundColor = color; }

		COLORREF GetBackgroundColor() { return m_backgroundColor; }

		EventCallback_t KeyDown{ NULL };
		EventCallback_t KeyUp{ NULL };
		EventCallback_t OnClose{ NULL };
	private:
		HWND m_hWnd;
		HICON m_hIcon;
		HFONT m_hFont;
		POINT m_minSize = { GetSystemMetrics(SM_CXMINTRACK), GetSystemMetrics(SM_CYMINTRACK) };
		POINT m_maxSize = { GetSystemMetrics(SM_CXMAXTRACK), GetSystemMetrics(SM_CYMAXTRACK) };
		COLORREF m_backgroundColor = RGB(255, 255, 255);
		std::shared_ptr<UIElement> m_content = NULL;

		static BOOL s_dpiAware;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}