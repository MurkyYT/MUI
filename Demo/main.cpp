#include "..\MUI\MUI.h"
#include "resource.h"
#include "GridDemoWindow.h"
#include "MainWindow.h"


INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
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