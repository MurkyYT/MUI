#include "..\MUI\MUI.h"
#include "resource.h"
#include "GridDemoWindow.h"
#include "MainWindow.h"


INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
   
    MainWindow* mainWind = new MainWindow();
    mainWind->Show();

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}