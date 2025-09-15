#pragma once

#include "UIElement.h"

namespace mui
{
    class NativeWindowHost : public UIElement
    {
    public:
        NativeWindowHost();
        ~NativeWindowHost();

        void SetHostedWindow(HWND hostedWindow);
        HWND GetHostedWindow() const { return m_hostedWindow; }

        void RemoveHostedWindow();

        size_t GetMinWidth() override;
        size_t GetMinHeight() override;

        EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void UpdateIdealSize() override;

        EventCallback_t OnResize{ NULL };

    private:
        void UpdateHostedWindowBounds();
        void SetHWND(HWND hWnd) override;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static ATOM RegisterWindowClass();

        HWND m_hostedWindow;
        BOOL m_hostedWindowSubclassed;
        SIZE m_defaultSize;

        BOOL m_needsUpdate;
    };
}