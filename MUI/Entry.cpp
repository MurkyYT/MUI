#include "Entry.h"

SIZE GetEditIdealSize(HWND hwndEdit)
{
    SIZE size = { 0 };

    HDC hdc = GetDC(hwndEdit);
    if (!hdc) return size;

    HFONT hFont = (HFONT)SendMessage(hwndEdit, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    int len = GetWindowTextLength(hwndEdit);
    std::wstring text(len, L'\0');
    GetWindowText(hwndEdit, &text[0], len + 1);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    int lineCount = (int)SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0);

    size.cy = tm.tmHeight * lineCount + 8;

    size.cx = 0;
    for (int i = 0; i < lineCount; ++i)
    {
        int lineLen = (int)SendMessage(hwndEdit, EM_LINELENGTH, SendMessage(hwndEdit, EM_LINEINDEX, i, 0), 0);
        std::wstring line(lineLen, L'\0');
        *((WORD*)&line[0]) = lineLen;
        SendMessage(hwndEdit, EM_GETLINE, i, (LPARAM)&line[0]);

        SIZE lineSize;
        GetTextExtentPoint32(hdc, line.c_str(), lineLen, &lineSize);

        if (lineSize.cx > size.cx)
            size.cx = lineSize.cx;
    }

    size.cx += 10;

    SelectObject(hdc, hOldFont);
    ReleaseDC(hwndEdit, hdc);

    return size;
}


mui::Entry::Entry(const wchar_t* text, int x, int y, int width, int height)
{
    m_name = text;
    m_class = L"Edit";
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_style = WS_VISIBLE | ES_LEFT | ES_WANTRETURN | ES_AUTOVSCROLL | WS_BORDER;
}

mui::Entry::Entry(const wchar_t* text, int x, int y) : Entry(text, x, y, 0, 0)
{
}

mui::Entry::Entry(const wchar_t* text) : Entry(text, 0, 0, 0, 0)
{
}

void mui::Entry::UpdateIdealSize()
{
    m_idealSize = GetEditIdealSize(m_hWnd);
}

void mui::Entry::SetTextColor(COLORREF color)
{
    m_color = color;
}

mui::UIElement::EventHandlerResult mui::Entry::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case EN_CHANGE:
        {
            if (this->TextChanged)
                this->TextChanged(this, { uMsg, wParam,lParam });

            int lineCount = (int)SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0);
            UpdateIdealSize();
            if (m_prevLineCount != lineCount)
                PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);

            m_prevLineCount = lineCount;
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_SIZE:
        UpdateIdealSize();
        break;
    case WM_CHAR:
    {
        LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_STYLE);
        if (wParam == VK_RETURN && !(style & ES_MULTILINE)) 
        {
            if (this->Completed)
                this->Completed(this, { uMsg, wParam,lParam });

            return { TRUE, 0 };
        }
    }
    break;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        ::SetTextColor((HDC)wParam, m_color);
        return { TRUE , NULL };
    default:
        break;
    }
    return { FALSE,NULL };
}

BOOL mui::Entry::SetText(const std::wstring& text)
{
    m_name = text;

    if (!m_hWnd || !m_parenthWnd)
        return TRUE;

    BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res;
}

BOOL mui::Entry::SetTextAligment(LayoutAligment aligment)
{
    m_style &= ~ES_CENTER;
    m_style &= ~ES_LEFT;
    m_style &= ~ES_RIGHT;

    switch (aligment)
    {
    case mui::Fill:
        return FALSE;
    case mui::Start:
        m_style |= ES_LEFT;
    case mui::End:
        m_style |= ES_RIGHT;
    case mui::Center:
        m_style |= ES_CENTER;
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

std::wstring mui::Entry::GetText()
{
    if(!m_hWnd)
        return m_name;

    std::wstring text;
    text.resize(GetWindowTextLengthW(m_hWnd) + 1);
    text.resize(GetWindowTextW(m_hWnd, (LPWSTR)text.data(), (int)text.size() + 1));
    return text;
}

BOOL mui::Entry::SetPlaceholder(const std::wstring& text)
{
    m_placeholder = text;

    if (!m_hWnd)
        return TRUE;

    return SendMessage(m_hWnd, EM_SETCUEBANNER, FALSE, (LPARAM)text.c_str()) > 0;
}

void mui::Entry::SetHWND(HWND hWnd)
{
    m_hWnd = hWnd;
    SendMessage(m_hWnd, EM_SETCUEBANNER, FALSE, (LPARAM)m_placeholder.c_str());
}

BOOL mui::Entry::SetMultiline(BOOL multiline)
{
    m_style &= ~ES_MULTILINE;

    if(multiline)
        m_style |= ES_MULTILINE;

    if (!m_hWnd)
        return TRUE;

    LONG_PTR res = SetWindowLongPtr(m_hWnd, GWL_STYLE, m_style | WS_CHILD);
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res > 0;
}