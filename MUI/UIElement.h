#pragma once

#include "Events.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <string>

#define MUI_WM_REDRAW (WM_APP + 7525)

namespace mui
{
	enum LayoutAlignment
	{
		Fill,
		Start,
		End,
		Center
	};

	class UIElement
	{
	public:
		virtual size_t GetMinWidth()
		{
			if (!m_visible)
				return 0;

			return m_idealSize.cx;
		}
		virtual size_t GetMinHeight()
		{
			if (!m_visible)
				return 0;

			return m_idealSize.cy;
		}
		virtual size_t GetMaxWidth() 
		{
			if (!m_visible)
				return 0;

			if (m_horizontalAlignment == Fill)
				return m_availableSize.right - m_availableSize.left;
			else
				return m_idealSize.cx;
		}
		virtual size_t GetMaxHeight()
		{
			if (!m_visible)
				return 0;

			if (m_verticalAlignment == Fill)
				return m_availableSize.bottom - m_availableSize.top;
			else
				return m_idealSize.cy;
		}
		virtual size_t GetHeight()
		{
			if (!m_visible)
				return 0;

			if (!m_hWnd)
				return m_height;

			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.bottom - rect.top;
		}
		virtual size_t GetWidth() 
		{
			if (!m_visible)
				return 0;

			if (!m_hWnd)
				return m_width;

			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.right - rect.left;
		}
		virtual size_t GetX()
		{
			if (!m_visible)
				return 0;

			if (!m_hWnd)
				return m_x;

			switch (m_horizontalAlignment)
			{
			case mui::Fill:
				return m_availableSize.left;
			case mui::Start:
				return m_availableSize.left;
			case mui::End:
				return (size_t)max(0, (int)(m_availableSize.right - (int)GetMinWidth()));
			case mui::Center:
				return (size_t)max(0, (int)((m_availableSize.right - m_availableSize.left) / 2 - (int)(GetMinWidth() / 2)));
			}

			return m_x;
		}

		virtual size_t GetY()
		{
			if (!m_visible)
				return 0;

			if (!m_hWnd)
				return m_y;

			switch (m_verticalAlignment)
			{
			case mui::Fill:
				return m_availableSize.top;
			case mui::Start:
				return m_availableSize.top;
			case mui::End:
				return (size_t)max(0, (int)(m_availableSize.bottom - (int)GetMinHeight()));
			case mui::Center:
				return (size_t)max(0, (int)((m_availableSize.bottom - m_availableSize.top) / 2 - (int)(GetMinHeight() / 2)));
			}

			return m_y;
		}

		void SetHorizontalAlignment(LayoutAlignment alignment) 
		{
			m_horizontalAlignment = alignment;
		}

		void SetVerticalAlignment(LayoutAlignment alignment)
		{
			m_verticalAlignment = alignment;
		}

		void SetEnabled(BOOL enabled)
		{
			m_enabled = enabled;

			EnableWindow(m_hWnd, enabled);
		}

		void SetVisible(BOOL visible)
		{
			m_visible = visible;

			ShowWindow(m_hWnd, visible ? SW_SHOWNA : SW_HIDE);
		}

		void SetBorder(BOOL enabled)
		{
			m_hasBorder = enabled;

			DWORD style = (DWORD)GetWindowLongPtr(m_hWnd, GWL_STYLE);

			if (enabled)
				style |= WS_BORDER;

			else
				style &= ~WS_BORDER;

			SetWindowLongPtr(m_hWnd, GWL_STYLE, style);


			SetWindowPos(
				m_hWnd,
				nullptr,
				0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
			);
		}

		HWND GetHWND() { return m_hWnd; }
		const wchar_t* GetClass() { return m_class.c_str(); }
		const wchar_t* GetName() { return m_name.c_str(); }
		DWORD GetStyle() { return m_style; }
		DWORD GetExStyle() { return m_exStyle; }
		LayoutAlignment GetVerticalAlignment() { return m_verticalAlignment; }
		LayoutAlignment GetHorizontalAlignment() { return m_horizontalAlignment; }
		BOOL IsEnabled() { return m_enabled; }

		virtual void SetBackgroundColor(COLORREF color) { m_backgroundColor = color; }

		COLORREF GetBackgroundColor()  { return m_backgroundColor; }

		DWORD GetID() { return m_id; }

		void SetID(DWORD id) { m_id = id; }

		void Initialize(HWND parenthWnd, DWORD id, HFONT font)
		{
			m_id = id;
			HWND hWnd = CreateWindowEx(
				m_exStyle,
				GetClass(),
				GetName(),
				m_style | WS_CHILD | (m_hasBorder ? WS_BORDER : 0),
				(int)GetX(), (int)GetY(), (int)GetWidth(), (int)GetHeight(),
				parenthWnd,
				(HMENU)(INT64)id,
				GetModuleHandle(NULL),
				this
			);

			SetHWND(hWnd);

			SendMessage(
				m_hWnd,
				WM_SETFONT,
				(WPARAM)font,
				TRUE
			);

			UpdateIdealSize();

			SetParentHWND(parenthWnd);

			SetWindowSubclass(m_hWnd, UIElement::CustomProc, (UINT_PTR)this, NULL);

			EnableWindow(m_hWnd, m_enabled);

			ShowWindow(m_hWnd, m_visible ? SW_SHOWNA : SW_HIDE);
		}

		void SetAvailableSize(RECT rect)
		{
			m_availableSize = rect;
		}

		struct EventHandlerResult
		{
			BOOL returnVal;
			LRESULT value;
		};

		virtual EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		EventCallback_t KeyDown{ NULL };
		EventCallback_t KeyUp{ NULL };

		EventCallback_t LeftMouseDown{ NULL };
		EventCallback_t LeftMouseUp{ NULL };
		EventCallback_t LeftMouseDoubleClick{ NULL };

		EventCallback_t RightMouseDown{ NULL };
		EventCallback_t RightMouseUp{ NULL };
		EventCallback_t RightMouseDoubleClick{ NULL };

		EventCallback_t MouseMove{ NULL };
		EventCallback_t MouseEnter{ NULL };
		EventCallback_t MouseLeave{ NULL };

		virtual void UpdateIdealSize() {};

	protected:

		virtual void SetHWND(HWND hWnd)
		{
			m_hWnd = hWnd;
		}

		virtual void SetParentHWND(HWND p_hWnd)
		{
			m_parenthWnd = p_hWnd;
		}

		~UIElement() { RemoveWindowSubclass(m_hWnd, CustomProc, (UINT_PTR)this); }

		LayoutAlignment m_verticalAlignment = Fill;
		LayoutAlignment m_horizontalAlignment = Fill;

		COLORREF m_backgroundColor = RGB(255, 255, 255);

		HWND m_parenthWnd = NULL;
		HWND m_hWnd = NULL;

		std::wstring m_class = L"";
		std::wstring m_name = L"";

		DWORD m_style = 0;
		DWORD m_exStyle = 0;
		DWORD m_id = 0;

		size_t m_x = 0;
		size_t m_y = 0;
		size_t m_width = 0;
		size_t m_height = 0;

		SIZE m_idealSize = { 0,0 };

		int m_minWidth = -1;
		int m_maxWidth = -1;

		int m_minHeight = -1;
		int m_maxHeight = -1;

		BOOL m_enabled = TRUE;
		BOOL m_mouseInside = FALSE;
		BOOL m_hasBorder = FALSE;
		BOOL m_visible = TRUE;

		RECT m_availableSize = {0,0,0,0};

		static LRESULT CALLBACK CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
			LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	};

}