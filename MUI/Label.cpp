#include "Label.h"

SIZE GetLabelIdealSize(HWND hwndLabel)
{
    HDC hdc = GetDC(hwndLabel);
    SIZE size = { 0 };

    if (hdc)
    {
        HFONT hFont = (HFONT)SendMessage(hwndLabel, WM_GETFONT, 0, 0);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        int len = GetWindowTextLength(hwndLabel);
        std::wstring text;
        text.reserve(len + 1);
        GetWindowText(hwndLabel, &text[0], len + 1);

        GetTextExtentPoint32(hdc, text.c_str(), len, &size);
        SelectObject(hdc, hOldFont);
        ReleaseDC(hwndLabel, hdc);
    }

    return size;
}

mui::Label::Label(const wchar_t* text, int x, int y, int width, int height)
{
    m_name = text;
    m_class = L"Static";
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_style = WS_VISIBLE | SS_LEFT;
}

mui::Label::Label(const wchar_t* text, int x, int y) : Label(text, x, y, 0, 0)
{
}

mui::Label::Label(const wchar_t* text) : Label(text, 0, 0, 0, 0)
{
}

void mui::Label::UpdateIdealSize()
{
	m_idealSize = GetLabelIdealSize(m_hWnd);
}

void mui::Label::SetTextColor(COLORREF color)
{
    m_color = color;
}

mui::UIElement::EventHandlerResult mui::Label::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
        ::SetTextColor((HDC)wParam, m_color);
        return { TRUE , NULL };
    default:
        break;
    }
	return { FALSE,NULL };
}

BOOL mui::Label::SetText(const std::wstring& text)
{
    m_name = text;

    if (!m_hWnd || !m_parenthWnd)
        return TRUE;

    BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res;
}

BOOL mui::Label::SetTextAligment(LayoutAligment aligment)
{
    m_style &= ~SS_CENTER;
    m_style &= ~SS_LEFT;
    m_style &= ~SS_RIGHT;

    switch (aligment)
    {
    case mui::Fill:
        return FALSE;
    case mui::Start:
        m_style |= SS_LEFT;
    case mui::End:
        m_style |= SS_RIGHT;
    case mui::Center:
        m_style |= SS_CENTER;
    default:
        break;
    }

    if (!m_hWnd)
        return TRUE;

    LONG_PTR res = SetWindowLongPtr(m_hWnd, GWL_STYLE, m_style | WS_CHILD);
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res > 0;
}

std::wstring mui::Label::GetText()
{
    return m_name;
}