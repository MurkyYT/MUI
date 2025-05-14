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
			PostMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_KEYUP:
			PostMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_DESTROY:
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, CustomProc, uIdSubclass);
			break;
		default:
		{
			EventHandlerResult res = element->HandleEvent(uMsg, wParam, lParam);
			if (res.returnVal)
				return res.value;
		}
		break;
		}
	}
		
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}