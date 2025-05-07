#include "CheckBox.h"

#include <uxtheme.h>
#include <vsstyle.h>
#include <tchar.h>
#include <dwmapi.h>

#ifndef TMT_CONTENTMARGINS
#define TMT_CONTENTMARGINS 3602
#endif

SIZE GetAccurateCheckboxSize(HWND hwnd)
{
    SIZE total = { 0, 0 };

    wchar_t text[256];
    GetWindowTextW(hwnd, text, 256);

    HDC hdc = GetDC(hwnd);
    HFONT hFont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    SIZE textSize = {};
    GetTextExtentPoint32W(hdc, text, lstrlenW(text), &textSize);
    SelectObject(hdc, oldFont);
    ReleaseDC(hwnd, hdc);

    int check = (int)SendMessageW(hwnd, BM_GETCHECK, 0, 0);
    BOOL enabled = IsWindowEnabled(hwnd);

    int stateId = CBS_UNCHECKEDNORMAL;
    switch (check)
    {
    case BST_CHECKED:
        stateId = enabled ? CBS_CHECKEDNORMAL : CBS_CHECKEDDISABLED;
        break;
    case BST_INDETERMINATE:
        stateId = enabled ? CBS_MIXEDNORMAL : CBS_MIXEDDISABLED;
        break;
    case BST_UNCHECKED:
    default:
        stateId = enabled ? CBS_UNCHECKEDNORMAL : CBS_UNCHECKEDDISABLED;
        break;
    }

    UINT dpi = 96;
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pGetDpiForWindow = (UINT(WINAPI*)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
        if (pGetDpiForWindow) dpi = pGetDpiForWindow(hwnd);
    }

    SIZE glyphSize = { 0, 0 };
    int spacing = 0;

    HTHEME hTheme = OpenThemeData(hwnd, L"BUTTON");
    if (hTheme)
    {
        GetThemePartSize(hTheme, NULL, BP_CHECKBOX, stateId, NULL, TS_TRUE, &glyphSize);

        MARGINS margins = {};
        if (SUCCEEDED(GetThemeMargins(hTheme, NULL, BP_CHECKBOX, stateId, TMT_CONTENTMARGINS, NULL, &margins)))
        {
            spacing = margins.cxLeftWidth;
        }

        CloseThemeData(hTheme);
    }

    if (glyphSize.cx == 0)
        glyphSize.cx = MulDiv(GetSystemMetrics(SM_CXMENUCHECK), dpi, 96);
    if (glyphSize.cy == 0)
        glyphSize.cy = MulDiv(GetSystemMetrics(SM_CYMENUCHECK), dpi, 96);
    if (spacing == 0)
        spacing = MulDiv(4, dpi, 96);

    total.cy = max(glyphSize.cy, textSize.cy);
    total.cx = glyphSize.cx + spacing + textSize.cx;

    if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
    {
        total.cx = textSize.cx + spacing + glyphSize.cx;
    }

    return total;
}

mui::CheckBox::CheckBox(const wchar_t* text, int x, int y, int width, int height)
{
	m_name = text;
	m_class = L"Button";
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_style = WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP;
}

mui::CheckBox::CheckBox(const wchar_t* text, int x, int y) : CheckBox(text, x, y, 0, 0)
{
}

mui::CheckBox::CheckBox(const wchar_t* text) : CheckBox(text, 0, 0, 0, 0)
{
}

size_t mui::CheckBox::GetMinHeight()
{
    SIZE size = GetAccurateCheckboxSize(m_hWnd);
	return size.cy;
}

size_t mui::CheckBox::GetMinWidth()
{
    SIZE size = GetAccurateCheckboxSize(m_hWnd);
	return size.cx;
}

size_t mui::CheckBox::GetMaxHeight()
{
    SIZE size = GetAccurateCheckboxSize(m_hWnd);
	if (m_verticalAligment == Fill)
		return max(size.cy, m_availableSize.bottom - m_availableSize.top);
	else
		return size.cy;
}

size_t mui::CheckBox::GetMaxWidth()
{
    SIZE size = GetAccurateCheckboxSize(m_hWnd);
	if (m_horizontalAligment == Fill)
		return max(size.cx, m_availableSize.right - m_availableSize.left);
	else
		return size.cx;
}

mui::UIElement::EventHandlerResult mui::CheckBox::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (this->StateChanged) StateChanged(this, { uMsg,wParam,lParam });
		}
		}
	}
    case WM_CTLCOLORSTATIC:
        return { TRUE , NULL };
	break;
	default:
		break;
	}

    return { FALSE, NULL };
}

BOOL mui::CheckBox::IsChecked()
{
	return IsDlgButtonChecked(m_parenthWnd, m_id);
}
void mui::CheckBox::SetChecked(BOOL checked)
{
	CheckDlgButton(m_parenthWnd, m_id, checked);
}