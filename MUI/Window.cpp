#include "Window.h"
#include <string>
#include <stdexcept>

mui::Window::Window(const wchar_t* title, size_t height, size_t width)
{
	std::wstring className = std::wstring(title) + std::to_wstring((ULONG_PTR)this);

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
	{
		window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (window) 
	{
		
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}