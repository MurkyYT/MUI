#include "CheckBox.h"

#include <uxtheme.h>
#include <vsstyle.h>
#include <tchar.h>
#include <dwmapi.h>

#pragma comment(lib, "uxtheme")
#pragma comment(lib, "dwmapi")

#ifndef TMT_CONTENTMARGINS
#define TMT_CONTENTMARGINS 3602
#endif

SIZE GetAccurateCheckboxSize(HWND hWnd)
{
    SIZE total = { 0, 0 };

    wchar_t text[256];
    GetWindowTextW(hWnd, text, 256);

    HDC hdc = GetDC(hWnd);
    HFONT hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    SIZE textSize = {};
    GetTextExtentPoint32W(hdc, text, lstrlenW(text), &textSize);
    SelectObject(hdc, oldFont);
    ReleaseDC(hWnd, hdc);

    int check = (int)SendMessageW(hWnd, BM_GETCHECK, 0, 0);
    BOOL enabled = IsWindowEnabled(hWnd);

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

    UINT dpi = GetDpiForWindow(hWnd);

    SIZE glyphSize = { 0, 0 };
    int spacing = 0;

    HTHEME hTheme = OpenThemeData(hWnd, L"BUTTON");
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
        break;
		}
	}
    break;
    case WM_CTLCOLORSTATIC:
        return { TRUE , NULL };
	default:
		break;
	}

    return { FALSE, NULL };
}

void mui::CheckBox::UpdateIdealSize()
{
    m_idealSize = GetAccurateCheckboxSize(m_hWnd);
}

BOOL mui::CheckBox::IsChecked()
{
	return IsDlgButtonChecked(m_parenthWnd, m_id);
}

void mui::CheckBox::SetChecked(BOOL checked)
{
    m_checked = checked;
	CheckDlgButton(m_parenthWnd, m_id, checked);
}

void mui::CheckBox::SetParentHWND(HWND hWnd)
{
    m_parenthWnd = hWnd;
    SetChecked(m_checked);
}

BOOL mui::CheckBox::SetText(const std::wstring& text)
{
    m_name = text;

    if (!m_hWnd || !m_parenthWnd)
        return TRUE;

    BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res;
}