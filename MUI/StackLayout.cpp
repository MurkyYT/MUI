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
	wcex.hbrBackground = NULL;
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
	m_uiType = UIType::StackLayout;
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
			0,
			element->GetClass(),
			element->GetName(),
			element->GetStyle() | WS_CHILD,
			(int)element->GetX(), (int)element->GetY(), (int)element->GetWidth(), (int)element->GetHeight(),
			m_hWnd,
			(HMENU)element->GetID(),
			GetModuleHandle(NULL),
			element.get()
		));
		SendMessage(
			element->GetHWND(),
			WM_SETFONT,
			(WPARAM)m_collection.GetFontHandle(),
			TRUE
		);

		element->SetParentHWND(m_hWnd);

		if (element->GetSubclass())
			SetWindowSubclass(element->GetHWND(), UIElement::CustomProc, (UINT_PTR)element.get(), NULL);

		ShowWindow(element->GetHWND(), SW_SHOW);
	}
	InvalidateRect(m_hWnd, NULL, TRUE);

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

void mui::StackLayout::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

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
		case WM_SIZE:
		{
			int x = 0;
			int y = 0;
			for (const std::shared_ptr<UIElement>& element : layout->m_collection.Items())
			{
				if (element->m_uiType == UIType::StackLayout)
					((StackLayout*)element.get())->m_insideAnotherStackLayout = TRUE;

				element->SetAvailableSize({x,y, layout->m_availableSize.right,layout->m_availableSize.bottom });

				if (layout->m_orientation == Vertical) 
				{
					size_t width = !layout->m_insideAnotherStackLayout &&
						element->m_uiType != UIType::StackLayout
						&& element->m_horizontalAligment == Fill && layout->m_orientation == Vertical
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
						element->m_uiType != UIType::StackLayout
						&& element->m_verticalAligment == Fill && layout->m_orientation == Horizontal
						? element->GetMaxHeight() : element->GetMinHeight();

					SetWindowPos(element->GetHWND(), NULL,
						x,
						layout->m_insideAnotherStackLayout ? y : (int)element->GetY(),
						(int)width,
						(int)height,
						NULL);

					x += (long)width;
				}
			}
		}
		break;
		case WM_COMMAND:
		{
			if (layout->Children().IDExists((DWORD)wParam))
				layout->Children().ItemByID((DWORD)wParam)->HandleEvent(uMsg, wParam, lParam);
		}
		break;
		case WM_NOTIFY:
		{
			if (layout->Children().IDExists((DWORD)((LPNMHDR)lParam)->idFrom))
				layout->Children().ItemByID((DWORD)((LPNMHDR)lParam)->idFrom)->HandleEvent(uMsg, wParam, lParam);
		}
		break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}