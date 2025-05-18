#include "Section.h"

#include <stdexcept>

mui::Section::Section(const std::wstring& text) : m_stack(Vertical)
{
	m_subclass = false;
	m_text = text;

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Section::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszClassName = L"MUI_Section";

	if (!RegisterClassEx(&wcex))
	{
		DWORD error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			throw std::runtime_error("Class creation failed");
	}

	m_class = L"MUI_Section";
	m_name = L"";

	m_style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	m_x = 0;
	m_y = 0;
}	

void mui::Section::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
	m_stack.SetHWND(CreateWindowEx(
		NULL,
		m_stack.GetClass(),
		m_stack.GetName(),
		m_stack.GetStyle() | WS_CHILD,
		(int)m_stack.GetX(), (int)m_stack.GetY(), (int)m_stack.GetWidth(), (int)m_stack.GetHeight(),
		m_hWnd,
		(HMENU)(UINT64)m_stack.GetID(),
		GetModuleHandle(NULL),
		&m_stack
	));
	SendMessage(
		m_stack.GetHWND(),
		WM_SETFONT,
		(WPARAM)m_stack.Children().GetFontHandle(),
		TRUE
	);

	m_stack.SetParentHWND(m_hWnd);

	m_stack.SetEnabled(m_enabled);

	ShowWindow(m_stack.GetHWND(), SW_SHOW);

	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);

	m_stack.Children().SetHWND(hWnd);
}

void mui::Section::UpdateIdealSize() 
{
	HDC hdc = GetDC(m_hWnd);

	SIZE size{};
	GetTextExtentPoint32(hdc, m_text.c_str(), (int)m_text.size(), &size);

	m_textSize = size;

	size_t minStackWidth = m_stack.GetMinWidth();

	size.cx = (LONG)(m_expanded ? max(minStackWidth + 16, size.cx + 16) : size.cx + 16);

	size.cy += (LONG)(m_expanded ? m_stack.GetMinHeight() : 0);

	m_idealSize = size;
}

mui::UIElement::EventHandlerResult mui::Section::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return { FALSE, NULL };
}

void mui::Section::DrawTriangle(HDC hdc, RECT rc, bool expanded, COLORREF backgroundColor, COLORREF triangleColor)
{
	POINT pts[3];

	HBRUSH hBrush = nullptr;

	if (expanded)
	{
		pts[0] = { rc.right - 5, rc.top + 5 };
		pts[1] = { rc.right - 5, rc.bottom - 6 };
		pts[2] = { rc.right - 10, rc.bottom - 6 };
		hBrush = CreateSolidBrush(triangleColor);
	}
	else
	{
		pts[0] = { rc.right - 10, rc.top + 3 };
		pts[1] = { rc.right - 6, (rc.top + rc.bottom) / 2 - 1 };
		pts[2] = { rc.right - 10, rc.bottom - 5 };
		hBrush = CreateSolidBrush(backgroundColor);
	}

	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 1, triangleColor);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	Polygon(hdc, pts, 3);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

LRESULT CALLBACK mui::Section::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Section* section;
	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		section = reinterpret_cast<Section*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(section));
		section->m_hWnd = hWnd;
	}
	else
		section = (Section*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (section)
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			RECT rc;
			GetClientRect(hWnd, &rc);
			HBRUSH hBrush = CreateSolidBrush(section->m_backgroundColor);
			FillRect(hdc, &rc, hBrush);

			DeleteObject(hBrush);
			return 1;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc = { 0,0,16,16 };
			section->DrawTriangle(hdc, rc, section->m_expanded, section->m_backgroundColor, section->m_expandColor);
			HFONT hOldFont = (HFONT)SelectObject(hdc, section->m_stack.m_collection.GetFontHandle());
			::SetTextColor(hdc, section->m_textColor);
			rc = { 16,0,section->m_idealSize.cx - 16,section->m_textSize.cy };
			DrawText(hdc, section->m_text.c_str(), (int)section->m_text.size(), &rc,DT_END_ELLIPSIS | DT_VCENTER | DT_LEFT);
			SelectObject(hdc, hOldFont);
			EndPaint(hWnd, &ps);

			if (section->m_expanded)
				SetWindowPos(section->m_stack.GetHWND(), NULL, 16, section->m_textSize.cy, (int)section->m_stack.GetMinWidth(), (int)section->m_stack.GetMinHeight(), NULL);
			else
				SetWindowPos(section->m_stack.GetHWND(), NULL, 16, section->m_textSize.cy, 0,0, NULL);
		}
		break;
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			SetFocus(hWnd);
			break;
		case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			HDC hdc = GetDC(hWnd);

			RECT windowRc{};
			GetClientRect(hWnd, &windowRc);
			RECT rc = { 0,0,windowRc.right - windowRc.left, section->m_textSize.cy };

			POINT pt = { x,y };

			if(PtInRect(&rc,pt))
				section->m_expanded = !section->m_expanded;

			if(section->m_expanded)
				section->m_stack.SetAvailableSize({ 0,0,  (long)section->m_stack.GetMinWidth(), (long)section->m_stack.GetMinHeight() });
			else
				section->m_stack.SetAvailableSize({ 0,0,0,0 });

			section->UpdateIdealSize();

			InvalidateRect(hWnd, NULL, TRUE);

			SetFocus(hWnd);

			PostMessage(section->m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
		}
			break;
		case WM_KEYDOWN:
			PostMessage(section->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_KEYUP:
			PostMessage(section->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case MUI_WM_REDRAW:
			PostMessage(section->m_parenthWnd, uMsg, wParam, lParam);
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void mui::Section::SetTextColor(COLORREF color)
{
	m_textColor = color;
}
void mui::Section::SetBackgroundColor(COLORREF color)
{
	m_backgroundColor = color;

	m_stack.SetBackgroundColor(color);
}
void mui::Section::SetExpandButtonColor(COLORREF color)
{
	m_expandColor = color;
}