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

void UpdateEditScrollbars(HWND hEdit)
{
    LONG_PTR style = GetWindowLongPtr(hEdit, GWL_STYLE);
    if (!(style & ES_MULTILINE)) return;

    BOOL currentVScroll = (style & WS_VSCROLL) != 0;
    BOOL currentHScroll = (style & WS_HSCROLL) != 0;

    HDC hdc = GetDC(hEdit);
    if (!hdc) return;

    HFONT hFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

    int lineHeight = tm.tmHeight + tm.tmExternalLeading;
    int lineCount = (int)SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
    int totalTextHeight = lineCount * lineHeight;

    RECT rc;
    GetClientRect(hEdit, &rc);
    BOOL needVScroll = totalTextHeight > (rc.bottom - rc.top);

    int maxLineWidth = 0;
    for (int i = 0; i < lineCount; ++i)
    {
        int lineStart = (int)SendMessage(hEdit, EM_LINEINDEX, i, 0);
        int lineLen = (int)SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);
        if (lineLen == 0) continue;

        std::wstring buffer(lineLen + 1, L'\0');
        *(WORD*)&buffer[0] = (WORD)lineLen;
        SendMessage(hEdit, EM_GETLINE, i, (LPARAM)buffer.data());

        SIZE size;
        GetTextExtentPoint32W(hdc, buffer.data(), lineLen, &size);

        if (size.cx > maxLineWidth)
            maxLineWidth = size.cx;
    }

    if (hOldFont) SelectObject(hdc, hOldFont);
    ReleaseDC(hEdit, hdc);

    BOOL needHScroll = maxLineWidth > (rc.right - rc.left);

    if (needVScroll != currentVScroll || needHScroll != currentHScroll)
    {
        ShowScrollBar(hEdit, SB_VERT, needVScroll);
        ShowScrollBar(hEdit, SB_HORZ, needHScroll);
        InvalidateRect(hEdit, NULL, FALSE);
    }
}

mui::Entry::Entry(const wchar_t* text, int x, int y, int width, int height)
{
    m_name = text;
    m_class = L"Edit";
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_style = WS_VISIBLE | ES_LEFT | ES_WANTRETURN | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_BORDER;
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
    m_textColor = color;
}

void mui::Entry::SetBackgroundColor(COLORREF color)
{
    DeleteObject(m_backroundBrush);
    m_backgroundColor = color;
    m_backroundBrush = CreateSolidBrush(m_backgroundColor);
}

mui::UIElement::EventHandlerResult mui::Entry::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static RECT prevAvail = m_availableSize;
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case EN_CHANGE:
        {
            EventArgs_t args = { uMsg, wParam, lParam, FALSE };
            if (this->TextChanged)
                this->TextChanged(this, &args);
            if (args.handled)
                break;

            SIZE prevIdeal = m_idealSize;
            UpdateIdealSize();

            static int prevLineCount = 0;
            static int prevMaxLineWidth = 0;

            int currentLineCount = (int)SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0);
            DWORD sel = (DWORD)SendMessage(m_hWnd, EM_GETSEL, 0, 0);
            int currentLine = (int)SendMessage(m_hWnd, EM_LINEFROMCHAR, LOWORD(sel), 0);
            int currentLineLen = (int)SendMessage(m_hWnd, EM_LINELENGTH, SendMessage(m_hWnd, EM_LINEINDEX, currentLine, 0), 0);

            if (currentLineCount != prevLineCount || currentLineLen > prevMaxLineWidth || currentLineLen < prevMaxLineWidth - 50) {
                UpdateEditScrollbars(m_hWnd);
                prevLineCount = currentLineCount;
                prevMaxLineWidth = currentLineLen;
            }

            if (!EqualRect(&prevAvail, &m_availableSize) &&
                (prevIdeal.cx != m_idealSize.cx || prevIdeal.cy != m_idealSize.cy))
            {
                PostMessage(m_parenthWnd, MUI_WM_REDRAW, (WPARAM)this, NULL);
            }
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_SIZE:
        UpdateEditScrollbars(m_hWnd);
        UpdateIdealSize();
        break;
    case WM_CHAR:
    {
        LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_STYLE);
        if (wParam == VK_RETURN && !(style & ES_MULTILINE)) 
        {
            EventArgs_t args = { uMsg, wParam,lParam, FALSE };
            if (this->Completed)
                this->Completed(this, &args);

            if (args.handled)
                return { TRUE, 0 };
        }
        else if(wParam == VK_RETURN && (style & ES_MULTILINE))
        {
            EventArgs_t args = { uMsg, wParam,lParam, FALSE };
            if (this->NewLine)
                this->NewLine(this, &args);

            if(args.handled)
                return { TRUE, 0 };
        }
        else if ((wParam == 19) && (GetKeyState(VK_CONTROL) & 0x8000))
        {
            EventArgs_t args = { uMsg, wParam,lParam, FALSE };
            if (this->Save)
                this->Save(this, &args);

            if (args.handled)
                return { TRUE, 0 };
        }
        else
        {
            EventArgs_t args = { uMsg, wParam,lParam, FALSE };
            if (this->CharPressed)
                this->CharPressed(this, &args);

            if (args.handled)
                return { TRUE, 0 };
        }
    }
    break;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        SetBkColor((HDC)wParam, m_backgroundColor);
        ::SetTextColor((HDC)wParam, m_textColor);
        return { TRUE , (LRESULT)m_backroundBrush};
    default:
        break;
    }
    prevAvail = m_availableSize;
    return { FALSE,NULL };
}

void mui::Entry::SetCaretPos(size_t pos)
{
    SendMessage(m_hWnd, EM_SETSEL, pos, pos);
    SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0);
}

void mui::Entry::SetIndentation(size_t indent)
{
    m_indentation = (int)indent;

    if (m_hWnd) 
    {
        int tabStops[] = { (int)indent };

        SendMessage(m_hWnd, EM_SETTABSTOPS, sizeof(tabStops) / sizeof(tabStops[0]), (LPARAM)tabStops);
        InvalidateRect(m_hWnd, NULL, TRUE);
    }
}

BOOL mui::Entry::SetText(const std::wstring& text)
{
    m_name = text;

    if (!m_hWnd || !m_parenthWnd)
        return TRUE;

    LockWindowUpdate(m_hWnd);
    BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
    UpdateEditScrollbars(m_hWnd);
    InvalidateRect(m_hWnd, NULL, TRUE);
    LockWindowUpdate(NULL);
    PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
    UpdateIdealSize();
    return res;
}

BOOL mui::Entry::SetTextAlignment(LayoutAlignment alignment)
{
    m_style &= ~ES_CENTER;
    m_style &= ~ES_LEFT;
    m_style &= ~ES_RIGHT;

    switch (alignment)
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

    LockWindowUpdate(m_hWnd);
    LONG_PTR res = SetWindowLongPtr(m_hWnd, GWL_STYLE, m_style | WS_CHILD);
    InvalidateRect(m_hWnd, NULL, TRUE);
    LockWindowUpdate(NULL);
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
    int tabStops[] = { m_indentation };

    PostMessage(m_hWnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 0);
    SendMessage(m_hWnd, EM_SETLIMITTEXT, 0x7FFFFFFE, 0);
    SendMessage(m_hWnd, EM_SETTABSTOPS, sizeof(tabStops) / sizeof(tabStops[0]), (LPARAM)tabStops);

    InvalidateRect(m_hWnd, NULL, TRUE);
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