#pragma once

#include <Windows.h>

namespace mui
{

	class UIElement
	{
		friend class Window;
		friend class UIElementCollection;
	private:
		HWND m_parenthWnd;
		HWND m_hWnd;

		wchar_t* m_class;
		wchar_t* m_name;

		DWORD m_style;
		DWORD m_id;

		size_t m_x;
		size_t m_y;
		size_t m_width;
		size_t m_height;

		bool m_subclass;

		static LRESULT CALLBACK CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam,
			LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	};

}