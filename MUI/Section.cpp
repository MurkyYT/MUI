#include "Section.h"

#include "Window.h"

#include <stdexcept>

mui::Section::Section(const std::wstring& text)
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

	NONCLIENTMETRICS ncm = {};
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
}	

void mui::Section::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;

	if (m_content) 
		m_content->Initialize(m_hWnd, m_content->GetID(), m_hFont);

	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
}

void mui::Section::UpdateIdealSize() 
{
	HDC hdc = GetDC(m_hWnd);

	HFONT hOldFont = (HFONT)SelectObject(hdc, m_hFont);

	SIZE size{};
	GetTextExtentPoint32(hdc, m_text.c_str(), lstrlenW(m_text.c_str()), &size);

	SelectObject(hdc, hOldFont);
	ReleaseDC(m_hWnd, hdc);

	m_textSize = size;

	UINT dpi = 96;
	HMODULE hUser32 = LoadLibraryA("User32.dll");
	if (hUser32) {
		auto pGetDpiForWindow = (decltype(&GetDpiForWindow))GetProcAddress(hUser32, "GetDpiForWindow");
		if (pGetDpiForWindow) {
			dpi = pGetDpiForWindow(m_hWnd);
		}
		FreeLibrary(hUser32);
	}

	size_t minWidth = m_content ? m_content->GetMinWidth() : 0;

	m_expandSize.cx = MulDiv(16, dpi, 96);
	m_expandSize.cy = MulDiv(16, dpi, 96);

	size.cx = (LONG)(m_expanded ? max(minWidth + m_expandSize.cx, size.cx + m_expandSize.cx) : size.cx + m_expandSize.cx);

	size.cy += (LONG)(m_expanded ? m_content->GetMinHeight() : 0);

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
		pts[0] = { (rc.right - rc.left) / 2 + MulDiv(2,m_expandSize.cx, 16) , (rc.bottom - rc.top) / 2 };
		pts[1] = { (rc.right - rc.left) / 2 + MulDiv(2,m_expandSize.cx, 16) , (rc.bottom - rc.top) / 2 + MulDiv(4,m_expandSize.cx, 16) };
		pts[2] = { (rc.right - rc.left) / 2 - MulDiv(2,m_expandSize.cx, 16) , (rc.bottom - rc.top) / 2 + MulDiv(4,m_expandSize.cx, 16) };
		hBrush = CreateSolidBrush(triangleColor);
	}
	else
	{
		pts[0] = { (rc.right - rc.left) / 2 - MulDiv(2,m_expandSize.cx, 16), (rc.bottom - rc.top) / 2 - MulDiv(4,m_expandSize.cx, 16) };
		pts[1] = { (rc.right - rc.left) / 2 + MulDiv(2,m_expandSize.cx, 16), (rc.bottom - rc.top) / 2 };
		pts[2] = { (rc.right - rc.left) / 2 - MulDiv(2,m_expandSize.cx, 16), (rc.bottom - rc.top) / 2 + MulDiv(4,m_expandSize.cx, 16) };
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
		case WM_DESTROY:
		{
			DeleteObject(section->m_hFont);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			break;
		}
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
			RECT rc = { 0,0,section->m_expandSize.cx,section->m_textSize.cy };
			section->DrawTriangle(hdc, rc, section->m_expanded, section->m_backgroundColor, section->m_expandColor);
			HFONT hOldFont = (HFONT)SelectObject(hdc, section->m_hFont);
			SetBkMode(hdc, TRANSPARENT);
			::SetTextColor(hdc, section->m_textColor);
			rc = { section-> m_expandSize.cx,0,section->m_idealSize.cx, section->m_textSize.cy };
			DrawText(hdc, section->m_text.c_str(), (int)section->m_text.size(), &rc,DT_END_ELLIPSIS | DT_VCENTER | DT_LEFT);
			SelectObject(hdc, hOldFont);
			EndPaint(hWnd, &ps);

			if (section->m_content)
			{
				if (section->m_expanded)
					SetWindowPos(section->m_content->GetHWND(), NULL, section->m_expandSize.cx, section->m_textSize.cy, (int)section->m_content->GetMinWidth(), (int)section->m_content->GetMinHeight(), NULL);
				else
					SetWindowPos(section->m_content->GetHWND(), NULL, section->m_expandSize.cx, section->m_textSize.cy, 0, 0, NULL);
			}
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

			if (section->m_content)
			{
				if (section->m_expanded)
					section->m_content->SetAvailableSize({ 0,0,  (long)section->m_content->GetMinWidth(), (long)section->m_content->GetMinHeight() });
				else
					section->m_content->SetAvailableSize({ 0,0,0,0 });
			}

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

	m_content->SetBackgroundColor(color);

	m_customBackground = TRUE;
}
void mui::Section::SetExpandButtonColor(COLORREF color)
{
	m_expandColor = color;
}