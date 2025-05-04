#include "UIElement.h"

#include <CommCtrl.h>

LRESULT CALLBACK mui::UIElement::CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UIElement* comp = (UIElement*)uIdSubclass;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		SendMessage(comp->m_parenthWnd, uMsg, wParam, lParam);
		break;
	case WM_KEYUP:
		SendMessage(comp->m_parenthWnd, uMsg, wParam, lParam);
		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, CustomProc, uIdSubclass);
		break;
	}
		
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}