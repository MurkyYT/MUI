#include "Window.h"
#include <CommCtrl.h>
#include <string>
#include <stdexcept>

mui::Window::Window(const wchar_t* title, size_t height, size_t width)
{
	std::wstring className = L"MUI_Window&" + std::to_wstring((ULONG_PTR)this);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszClassName = className.c_str();

	if (!RegisterClassEx(&wcex))
		throw std::runtime_error("Class creation failed");

	m_hWnd = CreateWindowEx
	(
		0,
		className.c_str(),
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(int)height,
		(int)width,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this
	);

	if (!m_hWnd) 
	{
		DWORD err = GetLastError();
		throw std::runtime_error("Window creation failed (" + std::to_string(err) + ")");
	}
}

void mui::Window::Show()
{
	ShowWindow(m_hWnd, SW_SHOW);
}

size_t mui::Window::GetHeight()
{
	RECT rect = GetRect();
	return rect.bottom - rect.top;
}

size_t mui::Window::GetWidth() 
{
	RECT rect = GetRect();
	return rect.right - rect.left;
}

POINT mui::Window::GetTopLeft()
{
	RECT rect = GetRect();
	return { rect.left, rect.top };
}

RECT mui::Window::GetRect()
{
	RECT rect{};
	GetWindowRect(m_hWnd, &rect);
	return rect;
}

void mui::Window::SetContent(const std::shared_ptr<UIElement>& element)
{
	if (m_content) 
		DestroyWindow(m_content->m_hWnd);

	element->m_parenthWnd = this->m_hWnd;
	element->m_id = (DWORD)1;
	HWND hWnd = CreateWindowEx(
		0,
		element->m_class,
		element->m_name,
		element->m_style | WS_CHILD,
		(int)element->m_x, (int)element->m_y, (int)element->m_width, (int)element->m_height,
		this->m_hWnd,
		(HMENU)1,
		GetModuleHandle(NULL),
		element.get()
	);

	element->SetHWND(hWnd);

	SendMessage(
		element->m_hWnd,
		WM_SETFONT,
		(WPARAM)this->m_hFont,
		TRUE
	);

	m_content = element;

	if(element->m_subclass)
		SetWindowSubclass(element->m_hWnd, UIElement::CustomProc, (UINT_PTR)element.get(), NULL);

	ShowWindow(element->m_hWnd, SW_SHOW);
}

LRESULT CALLBACK mui::Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	Window* window;
	if (uMsg == WM_NCCREATE) 
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->m_hWnd = hWnd;
	}
	else 
		window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (window) 
	{
		switch (uMsg)
		{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			DeleteObject(window->m_hFont);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			window->m_content = NULL;
		}
		break;
		case WM_SIZE:
		{
			if(window->m_content)
			{
				RECT rect{};
				GetClientRect(hWnd, &rect);
				window->m_content->m_availableSize = rect;
				SetWindowPos(window->m_content->m_hWnd, NULL, 
					(int)window->m_content->GetX(),
					(int)window->m_content->GetY(),
					(int)window->m_content->GetMaxWidth(),
					(int)window->m_content->GetMaxHeight() , 
					NULL);
			}
		}
		break;
		case WM_COMMAND:
		{
			if (window->m_content && wParam == window->m_content->m_id)
				window->m_content->HandleEvent(uMsg, wParam, lParam);
		}
		break;
		case WM_NOTIFY:
		{
			if (window->m_content && ((LPNMHDR)lParam)->idFrom == window->m_content->m_id)
				window->m_content->HandleEvent(uMsg, wParam, lParam);
		}
		break;
		case WM_CREATE:
		{
			NONCLIENTMETRICS ncm = {};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			window->m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
		}
		break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}