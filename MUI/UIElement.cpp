#include "UIElement.h"

#include <CommCtrl.h>

LRESULT CALLBACK mui::UIElement::CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UIElement* element = (UIElement*)uIdSubclass;

	if (element) 
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			SendMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_KEYUP:
			SendMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, CustomProc, uIdSubclass);
			break;
		default:
			element->HandleEvent(uMsg, wParam, lParam);
			break;
		}
	}
		
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}