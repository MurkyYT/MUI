#include "Window.h"
#include <CommCtrl.h>
#include <string>
#include <stdexcept>
#include <Rpc.h>

#pragma comment (lib, "comctl32")
#pragma comment (lib, "rpcrt4")

BOOL mui::Window::s_dpiAware = SetProcessDPIAware();

mui::Window::Window(const wchar_t* title, size_t height, size_t width)
{
	if(!s_dpiAware)
	 s_dpiAware = SetProcessDPIAware();

	UUID uuid;
	RPC_STATUS status = UuidCreate(&uuid);

	if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY)
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(status);
		throw std::runtime_error(err);
		return;
	}

	RPC_WSTR strUuid = NULL;
	status = UuidToString(&uuid, &strUuid);

	if (status != RPC_S_OK)
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(status);
		throw std::runtime_error(err);
		return;
	}

	std::wstring className = L"MUI_Window [" + std::wstring((const wchar_t*)strUuid) + L"]";

	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);

	ExtractIconExW(buffer, 0, &m_hIcon, NULL, 1);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszClassName = className.c_str();
	wcex.hIcon = m_hIcon;

	if (!RegisterClassEx(&wcex)) 
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(GetLastError());
		throw std::runtime_error(err);
		return;
	}

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
		return;
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
		DestroyWindow(m_content->GetHWND());

	m_content = element;

	element->Initialize(m_hWnd, (DWORD)1, m_hFont);
}

BOOL mui::Window::SetTitle(const wchar_t* title)
{
	return SetWindowText(m_hWnd, title);
}

std::wstring mui::Window::GetTitle()
{
	std::wstring text;
	text.resize(GetWindowTextLengthW(m_hWnd) + 1);
	text.resize(GetWindowTextW(m_hWnd, (LPWSTR)text.data(), (int)text.size() + 1));
	return text;
}

void mui::Window::Close()
{
	DestroyWindow(m_hWnd);
}

void mui::Window::SetMaxWidth(size_t width)
{
	m_maxSize.x = (LONG)width;
}

void mui::Window::SetMaxHeight(size_t height)
{
	m_maxSize.y = (LONG)height;
}

void mui::Window::SetMinWidth(size_t width)
{
	m_minSize.x = (LONG)width;
}

void mui::Window::SetMinHeight(size_t height)
{
	m_minSize.y = (LONG)height;
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
		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			RECT rc;
			GetClientRect(hWnd, &rc);
			HBRUSH hBrush = CreateSolidBrush(window->m_backgroundColor);
			FillRect(hdc, &rc, hBrush);

			DeleteObject(hBrush);
			return 1;
		}
		case WM_DESTROY:
		case WM_CLOSE:
		{
			DeleteObject(window->m_hFont);
			DestroyIcon(window->m_hIcon);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			if(window->m_content)
				SetWindowLongPtr(window->m_content->GetHWND(), GWLP_USERDATA, NULL);
			window->m_content = NULL;
		}
		break;
		case MUI_WM_REDRAW:
			// Intentional fall through
		case WM_SIZE:
		{
			if(window->m_content)
			{
				LockWindowUpdate(window->m_content->GetHWND());
				RECT rect{};
				GetClientRect(hWnd, &rect);
				window->m_content->SetAvailableSize(rect);
				SetWindowPos(window->m_content->GetHWND(), NULL,
					(int)window->m_content->GetX(),
					(int)window->m_content->GetY(),
					(int)window->m_content->GetMaxWidth(),
					(int)window->m_content->GetMaxHeight() , 
					NULL);
				InvalidateRect(window->m_content->GetHWND(), NULL, TRUE);
				LockWindowUpdate(NULL);
			}
		}
		break;
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize = window->m_minSize;
			lpMMI->ptMaxTrackSize = window->m_maxSize;
			break;
		}
		case WM_COMMAND:
		{
			if (window->m_content && LOWORD(wParam) == window->m_content->GetID())
			{
				UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}	
		}
		break;
		case WM_NOTIFY:
		{
			if (window->m_content && ((LPNMHDR)lParam)->idFrom == window->m_content->GetID())
			{
				UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			HWND hwnd = (HWND)lParam;
			if (hwnd != NULL) {
				int controlId = GetDlgCtrlID(hwnd);
				if (window->m_content && controlId == window->m_content->GetID())
				{
					mui::UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
					if (res.returnVal)
						return res.value;
				}
			}
		}
		case WM_CREATE:
		{
			NONCLIENTMETRICS ncm = {};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			window->m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
		}
		break;
		case WM_KEYDOWN:
		{
			if (window->KeyDown)
				window->KeyDown(window, { uMsg, wParam, lParam });
		}
		break;
		case WM_KEYUP:
		{
			if (window->KeyUp)
				window->KeyUp(window, { uMsg, wParam, lParam });
		}
		break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}