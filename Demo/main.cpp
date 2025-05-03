#include <MUI.h>

using namespace mui;

INT WINAPI WinMain(\
	_In_ HINSTANCE hInstance, \
	_In_opt_ HINSTANCE hPrevInstance, \
	_In_ LPSTR lpCmdLine, \
	_In_ int nShowCmd)
{

	Window wind(L"Test");
	wind.Show();

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}
