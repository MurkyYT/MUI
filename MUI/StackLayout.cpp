#include "StackLayout.h"

#include <stdexcept>
#include <CommCtrl.h>

mui::StackLayout::StackLayout(StackLayoutOrientation orientation)
{
	m_subclass = false;
	m_orientation = orientation;

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = StackLayout::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
#ifndef NDEBUG
	wcex.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
#else
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
#endif
	wcex.lpszClassName = L"MUI_StackLayout";

	if (!RegisterClassEx(&wcex))
	{
		DWORD error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			throw std::runtime_error("Class creation failed");
	}

	m_class = L"MUI_StackLayout";
	m_name = L"";

	m_style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	m_x = 0;
	m_y = 0;
}

void mui::StackLayout::SetParentHWND(HWND p_hWnd)
{
	wchar_t buf[256];
	GetClassNameW(p_hWnd, buf, 256);

	if (!lstrcmpW(buf, L"MUI_StackLayout"))
		m_insideAnotherStackLayout = TRUE;

	m_parenthWnd = p_hWnd;
}

mui::UIElementCollection& mui::StackLayout::Children()
{
	return m_collection;
}

void mui::StackLayout::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
	for (const std::shared_ptr<UIElement>& element : m_collection.Items())
	{
		element->SetHWND(CreateWindowEx(
			element->m_exStyle,
			element->GetClass(),
			element->GetName(),
			element->GetStyle() | WS_CHILD,
			(int)element->GetX(), (int)element->GetY(), (int)element->GetWidth(), (int)element->GetHeight(),
			m_hWnd,
			(HMENU)(UINT64)element->GetID(),
			GetModuleHandle(NULL),
			element.get()
		));
		SendMessage(
			element->GetHWND(),
			WM_SETFONT,
			(WPARAM)m_collection.GetFontHandle(),
			TRUE
		);

		element->UpdateIdealSize();

		element->SetParentHWND(m_hWnd);

		if (element->GetSubclass())
			SetWindowSubclass(element->GetHWND(), UIElement::CustomProc, (UINT_PTR)element.get(), NULL);

		ShowWindow(element->GetHWND(), SW_SHOW);
	}
	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);

	m_collection.SetHWND(hWnd);
}

size_t mui::StackLayout::CalcMinHeight()
{
	long height = 0;
	if (m_orientation == StackLayoutOrientation::Vertical)
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
			height += (long)element->GetMinHeight();
	}
	else
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			size_t temp = element->GetMinHeight();
			if (temp > height)
				height = (long)temp;
		}
	}
	return height;
}

size_t mui::StackLayout::CalcMinWidth()
{
	long width = 0;
	if (m_orientation == StackLayoutOrientation::Horizontal)
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
			width += (long)element->GetMinWidth();
	}
	else
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			size_t temp = element->GetMinWidth();
			if (temp > width)
				width = (long)temp;
		}
	}
	return width;
}

size_t mui::StackLayout::CalcMaxHeight()
{
	long height = 0;
	if (m_orientation == StackLayoutOrientation::Vertical)
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			element->SetAvailableSize({ 0,height,m_availableSize.right, m_availableSize.bottom });
			height += (long)element->GetMaxHeight();
		}
	}
	else
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			element->SetAvailableSize(m_availableSize);
			size_t temp = element->GetMaxHeight();
			if (temp > height)
				height = (long)temp;
		}
	}
	return max(height, m_availableSize.bottom - m_availableSize.top);
}

size_t mui::StackLayout::CalcMaxWidth()
{
	long width = 0;
	if (m_orientation == StackLayoutOrientation::Horizontal)
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			element->SetAvailableSize({ width,0 ,m_availableSize.right, m_availableSize.bottom });
			width += (long)element->GetMaxWidth();
		}
	}
	else
	{
		for (const std::shared_ptr<UIElement>& element : m_collection.Items())
		{
			element->SetAvailableSize(m_availableSize);
			size_t temp = element->GetMaxWidth();
			if (temp > width)
				width = (long)temp;
		}
	}
	return max(width, m_availableSize.right - m_availableSize.left);
}

mui::UIElement::EventHandlerResult mui::StackLayout::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return { FALSE, NULL };
}

LRESULT CALLBACK mui::StackLayout::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StackLayout* layout;
	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		layout = reinterpret_cast<StackLayout*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(layout));
		layout->m_hWnd = hWnd;
	}
	else
		layout = (StackLayout*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (layout)
	{
		switch (uMsg)
		{
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
			SetFocus(hWnd);
			break;
		case WM_KEYDOWN:
			PostMessage(layout->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_KEYUP:
			PostMessage(layout->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case MUI_WM_REDRAW:
		{
			UIElement* element = (UIElement*)wParam;
			if(element)
			{
				if (layout->m_orientation == Vertical)
				{
					size_t width = !layout->m_insideAnotherStackLayout &&
						lstrcmpW(element->GetClass(), L"MUI_StackLayout") != 0
						&& element->m_horizontalAligment == Fill
						? element->GetMaxWidth() : element->GetMinWidth();

					size_t height = element->GetMinHeight();

					if (height == element->GetHeight() && width == element->GetWidth())
						break;
				}
				else
				{
					size_t width = element->GetMinWidth();
					size_t height = !layout->m_insideAnotherStackLayout &&
						lstrcmpW(element->GetClass(), L"MUI_StackLayout") != 0
						&& element->m_verticalAligment == Fill
						? element->GetMaxHeight() : element->GetMinHeight();
					
					if (height == element->GetHeight() && width == element->GetWidth())
						break;
				}
			}
			PostMessage(layout->m_parenthWnd, uMsg, wParam, lParam);
		}
			// Intentional fall through
		case WM_SIZE:
		{
			int x = 0;
			int y = 0;
			for (const std::shared_ptr<UIElement>& element : layout->m_collection.Items())
			{
				element->SetAvailableSize({ x,y, layout->m_availableSize.right,layout->m_availableSize.bottom });

				if (layout->m_orientation == Vertical)
				{
					size_t width = !layout->m_insideAnotherStackLayout &&
						lstrcmpW(element->GetClass(), L"MUI_StackLayout") != 0
						&& element->m_horizontalAligment == Fill
						? element->GetMaxWidth() : element->GetMinWidth();

					size_t height = element->GetMinHeight();

					SetWindowPos(element->GetHWND(), NULL,
						layout->m_insideAnotherStackLayout ? x : (int)element->GetX(),
						y,
						(int)width,
						(int)height,
						NULL);

					y += (long)height;
				}
				else
				{
					size_t width = element->GetMinWidth();
					size_t height = !layout->m_insideAnotherStackLayout &&
						lstrcmpW(element->GetClass(), L"MUI_StackLayout") != 0
						&& element->m_verticalAligment == Fill
						? element->GetMaxHeight() : element->GetMinHeight();

					SetWindowPos(element->GetHWND(), NULL,
						x,
						layout->m_insideAnotherStackLayout ? y : (int)element->GetY(),
						(int)width,
						(int)height,
						NULL);

					x += (long)width;
				}

				InvalidateRect(element->GetHWND(), NULL, TRUE);
			}
		}
		break;
		case WM_COMMAND:
		{
			if (layout->Children().IDExists((DWORD)LOWORD(wParam)))
			{
				EventHandlerResult res = layout->Children().ItemByID((DWORD)LOWORD(wParam))->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_NOTIFY:
		{
			if (layout->Children().IDExists((DWORD)((LPNMHDR)lParam)->idFrom))
			{
				EventHandlerResult res = layout->Children().ItemByID((DWORD)((LPNMHDR)lParam)->idFrom)->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC: 
		{
			HWND hwnd = (HWND)lParam;
			if (hwnd != NULL) {
				int controlId = GetDlgCtrlID(hwnd);
				if (layout->Children().IDExists(controlId))
				{
					EventHandlerResult res = layout->Children().ItemByID(controlId)->HandleEvent(uMsg, wParam, lParam);
					if (res.returnVal)
						return res.value;
				}
			}
		}
		break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}