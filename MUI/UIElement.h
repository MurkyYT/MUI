#pragma once

#include <Windows.h>

namespace mui
{

	class UIElement
	{
		friend class Window;
		friend class UIElementCollection;
	public:
		virtual void HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		virtual size_t GetMinWidth() = 0;
		virtual size_t GetMinHeight() = 0;
		virtual size_t GetMaxWidth() = 0;
		virtual size_t GetMaxHeight() = 0;
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

			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.left;
		}
		virtual size_t GetY()
		{
			if (!m_hWnd)
				return m_y;

			RECT rect{};
			GetWindowRect(m_hWnd, &rect);
			return rect.top;
		}

		virtual void SetHWND(HWND hWnd)
		{
			m_hWnd = hWnd;
		}

		void SetParentHWND(HWND p_hWnd)
		{
			m_parenthWnd = p_hWnd;
		}

		void SetAvailableSize(RECT rect)
		{
			m_availableSize = rect;
		}

		HWND GetHWND() { return m_hWnd; }
		const wchar_t* GetClass() { return m_class; }
		const wchar_t* GetName() { return m_name; }
		DWORD GetStyle() { return m_style; }
		DWORD GetID() { return m_id; }
		BOOL GetSubclass() { return m_subclass; }

	protected:
		HWND m_parenthWnd = NULL;
		HWND m_hWnd = NULL;

		const wchar_t* m_class = L"";
		const wchar_t* m_name = L"";

		DWORD m_style = 0;
		DWORD m_id = 0;

		size_t m_x = 0;
		size_t m_y = 0;
		size_t m_width = 0;
		size_t m_height = 0;

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