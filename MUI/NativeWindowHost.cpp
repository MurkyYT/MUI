#include "NativeWindowHost.h"
#include <CommCtrl.h>
#include <stdexcept>

mui::NativeWindowHost::NativeWindowHost()
    : m_hostedWindow(nullptr)
    , m_hostedWindowSubclassed(FALSE)
    , m_needsUpdate(FALSE)
{
    RegisterWindowClass();

    m_class = L"MUI_NativeWindowHost";
    m_name = L"";

    m_style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
    m_exStyle = 0;

    m_defaultSize = { 0,0 };
    m_idealSize = m_defaultSize;
}

ATOM mui::NativeWindowHost::RegisterWindowClass() {
    static ATOM atom = 0;
    if (atom != 0)
        return atom;

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = NativeWindowHost::WindowProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;
    wcex.lpszClassName = L"MUI_NativeWindowHost";

    atom = RegisterClassEx(&wcex);
    if (!atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        throw std::runtime_error("Class creation failed");
    }
    return atom;
}

mui::NativeWindowHost::~NativeWindowHost()
{
    RemoveHostedWindow();
}

LRESULT CALLBACK mui::NativeWindowHost::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    NativeWindowHost* host;
    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        host = reinterpret_cast<NativeWindowHost*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(host));
        host->m_hWnd = hWnd;
    }
    else
        host = (NativeWindowHost*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (host)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        break;
        case WM_SIZE:
            if (host->m_hostedWindow && IsWindow(host->m_hostedWindow))
                host->UpdateHostedWindowBounds();
            break;
        case WM_MOVE:
            if (host->m_hostedWindow && IsWindow(host->m_hostedWindow))
                host->UpdateHostedWindowBounds();
            break;
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hWnd, &rc);
            HBRUSH hBrush = CreateSolidBrush(host->m_backgroundColor);
            FillRect(hdc, &rc, hBrush);

            DeleteObject(hBrush);
            return 1;
        }
        case MUI_WM_REDRAW:
            if (host->m_hostedWindow && IsWindow(host->m_hostedWindow))
                host->UpdateHostedWindowBounds();

            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

mui::UIElement::EventHandlerResult mui::NativeWindowHost::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return { FALSE, NULL };
}

void mui::NativeWindowHost::SetHostedWindow(HWND hostedWindow)
{
    if (m_hostedWindow == hostedWindow)
        return;

    RemoveHostedWindow();

    m_hostedWindow = hostedWindow;

    if (m_hostedWindow && IsWindow(m_hostedWindow))
    {
        HWND hPrevFocus = GetFocus();
        SetParent(m_hostedWindow, m_hWnd);
        if (hPrevFocus)
            SetFocus(hPrevFocus);
        ShowWindow(m_hostedWindow, SW_SHOWNA);
        UpdateIdealSize();
        UpdateHostedWindowBounds();
    }
}

void mui::NativeWindowHost::RemoveHostedWindow()
{
    if (m_hostedWindow)
    {
        if (IsWindow(m_hostedWindow))
            SetParent(m_hostedWindow, GetDesktopWindow());

        m_hostedWindow = NULL;

        m_idealSize = m_defaultSize;

        if (m_hWnd)
            InvalidateRect(m_hWnd, NULL, TRUE);
    }
}

size_t mui::NativeWindowHost::GetMinWidth()
{
    if (!m_visible)
        return 0;

    if (m_hostedWindow && IsWindow(m_hostedWindow))
    {
        RECT rect;
        if (GetWindowRect(m_hostedWindow, &rect))
            return rect.right - rect.left;
    }
    return m_idealSize.cx;
}

size_t mui::NativeWindowHost::GetMinHeight()
{
    if (!m_visible)
        return 0;

    if (m_hostedWindow && IsWindow(m_hostedWindow))
    {
        RECT rect;
        if (GetWindowRect(m_hostedWindow, &rect))
            return rect.bottom - rect.top;
    }
    return m_idealSize.cy;
}

void mui::NativeWindowHost::UpdateIdealSize()
{
    if (m_hostedWindow && IsWindow(m_hostedWindow))
    {
        RECT rect;
        if (GetWindowRect(m_hostedWindow, &rect))
        {
            m_idealSize.cx = rect.right - rect.left;
            m_idealSize.cy = rect.bottom - rect.top;
        }
    }
    else
        m_idealSize = m_defaultSize;
}

void mui::NativeWindowHost::SetHWND(HWND hWnd)
{
    if (m_hostedWindow && IsWindow(m_hostedWindow))
    {
        SetParent(m_hostedWindow, m_hWnd);
        UpdateHostedWindowBounds();
    }
}

void mui::NativeWindowHost::UpdateHostedWindowBounds()
{
    if (!m_hostedWindow || !IsWindow(m_hostedWindow) || !m_hWnd)
        return;
    if (this->OnResize)
        OnResize(this, NULL);
}