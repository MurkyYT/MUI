#pragma once

#include <Windows.h>

#include <CommCtrl.h>

#include <string>

#define MUI_WM_REDRAW (WM_APP + 7525)

namespace mui
{
	enum LayoutAligment
	{
		Fill,
		Start,
		End,
		Center
	};

	class UIElement
	{
		friend class Window;
		friend class StackLayout;
		friend class UIElementCollection;

	public:
		virtual size_t GetMinWidth()
		{
			return m_idealSize.cx;
		}
		virtual size_t GetMinHeight()
		{
			return m_idealSize.cy;
		}
		virtual size_t GetMaxWidth() 
		{
			if (m_horizontalAligment == Fill)
				return m_availableSize.right - m_availableSize.left;
			else
				return m_idealSize.cx;
		}
		virtual size_t GetMaxHeight()
		{
			if (m_verticalAligment == Fill)
				return m_availableSize.bottom - m_availableSize.top;
			else
				return m_idealSize.cy;
		}
		virtual size_t GetHeight()
		{
			if (!m_hWnd)
				return m_height;
			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.bottom - rect.top;
		}
		virtual size_t GetWidth() 
		{
			if (!m_hWnd)
				return m_width;
			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.right - rect.left;
		}
		virtual size_t GetX()
		{
			if (!m_hWnd)
				return m_x;

			switch (m_horizontalAligment)
			{
			case mui::Fill:
				return m_availableSize.left;
			case mui::Start:
				return m_availableSize.left;
			case mui::End:
				return m_availableSize.right - GetMinWidth();
			case mui::Center:
				return (m_availableSize.right - m_availableSize.left) / 2 - GetMinWidth() / 2;
			}

			return m_x;
		}
		virtual size_t GetY()
		{
			if (!m_hWnd)
				return m_y;

			switch (m_verticalAligment)
			{
			case mui::Fill:
				return m_availableSize.top;
			case mui::Start:
				return m_availableSize.top;
			case mui::End:
				return m_availableSize.bottom - GetMinHeight();
			case mui::Center:
				return (m_availableSize.bottom - m_availableSize.top) / 2 - GetMinHeight() / 2;
			}

			return m_y;
		}

		void SetHorizontalAligment(LayoutAligment aligment) 
		{
			m_horizontalAligment = aligment;
		}

		void SetVerticalAligment(LayoutAligment aligment)
		{
			m_verticalAligment = aligment;
		}

		HWND GetHWND() { return m_hWnd; }
		const wchar_t* GetClass() { return m_class.c_str(); }
		const wchar_t* GetName() { return m_name.c_str(); }
		DWORD GetStyle() { return m_style; }
		DWORD GetID() { return m_id; }
		BOOL GetSubclass() { return m_subclass; }

	protected:

		~UIElement() { if (m_subclass) RemoveWindowSubclass(m_hWnd, CustomProc, (UINT_PTR)this); }

		struct EventHandlerResult
		{
			BOOL returnVal;
			LRESULT value;
		};

		virtual EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		virtual void SetHWND(HWND hWnd)
		{
			m_hWnd = hWnd;
		}

		virtual void UpdateIdealSize() {};

		virtual void SetParentHWND(HWND p_hWnd)
		{
			m_parenthWnd = p_hWnd;
		}

		void SetAvailableSize(RECT rect)
		{
			m_availableSize = rect;
		}

		LayoutAligment m_verticalAligment = Fill;
		LayoutAligment m_horizontalAligment = Fill;

		HWND m_parenthWnd = NULL;
		HWND m_hWnd = NULL;

		std::wstring m_class = L"";
		std::wstring m_name = L"";

		DWORD m_style = 0;
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

		BOOL m_subclass = TRUE;

		RECT m_availableSize = {0,0,0,0};

		static LRESULT CALLBACK CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
			LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	};

}