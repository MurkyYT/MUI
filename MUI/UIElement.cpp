#include "UIElement.h"

#include <CommCtrl.h>

LRESULT CALLBACK mui::UIElement::CustomProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UIElement* element = (UIElement*)uIdSubclass;

	if (element)
	{
		EventArgs_t args = { uMsg, wParam, lParam, FALSE };
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			if (!element->m_mouseInside)
			{
				element->m_mouseInside = true;

				TRACKMOUSEEVENT tme = {};
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				TrackMouseEvent(&tme);

				if (element->MouseEnter)
				{
					EventArgs_t enterArgs = { NULL, wParam, lParam, FALSE };
					element->MouseEnter(element, &enterArgs);
				}
			}

			if (element->MouseMove)
				element->MouseMove(element, &args);
			if (args.handled)
				return args.msg;
			else
				break;

		case WM_MOUSELEAVE:
			element->m_mouseInside = false;
			if (element->MouseLeave)
				element->MouseLeave(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_LBUTTONDOWN:
			if (element->LeftMouseDown)
				element->LeftMouseDown(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_LBUTTONUP:
			if (element->LeftMouseUp)
				element->LeftMouseUp(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_LBUTTONDBLCLK:
			if (element->LeftMouseDoubleClick)
				element->LeftMouseDoubleClick(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_RBUTTONDOWN:
			if (element->RightMouseDown)
				element->RightMouseDown(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_RBUTTONUP:
			if (element->RightMouseUp)
				element->RightMouseUp(element, &args);
			break;
		case WM_RBUTTONDBLCLK:
			if (element->RightMouseDoubleClick)
				element->RightMouseDoubleClick(element, &args);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (element->KeyDown)
				element->KeyDown(element, &args);

			if (!args.handled)
				PostMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if (element->KeyUp)
				element->KeyUp(element, &args);

			if (!args.handled)
				PostMessage(element->m_parenthWnd, uMsg, wParam, lParam);
			if(args.handled)
				return args.msg;
			else
				break;
		case WM_DESTROY:
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, CustomProc, uIdSubclass);
			break;
		default:
			break;
		}

		EventHandlerResult res = element->HandleEvent(uMsg, wParam, lParam);
		if (res.returnVal)
			return res.value;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}