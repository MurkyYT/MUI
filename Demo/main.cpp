#include <MUI.h>

using namespace mui;

std::shared_ptr<StackLayout> layout3 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
std::shared_ptr<StackLayout> layout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);
std::shared_ptr<Button> button2 = std::make_shared<Button>(L"Test budsadsadtton2", 0, 0, 200, 200);

void BtnClick()
{
	std::shared_ptr<Button> button = std::make_shared<Button>(L"Test button", 0, 0, 200, 200);
	layout3->Children().Add(button);
	layout->Children().Remove(button2);
}

INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Window wind(L"Test");
	
	
	std::shared_ptr<StackLayout> layout2 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
	
	
	std::shared_ptr<Button> button3 = std::make_shared<Button>(L"Test button3", 0, 0, 200, 200);
	for (int i = 0; i < 20; i++) {
		std::shared_ptr<Button> button4 = std::make_shared<Button>(L"Testdsadsadsadsa button3", 0, 0, 200, 200);
		button4->OnClick = std::bind(BtnClick);
		layout2->Children().Add(button4);
	}

	button2->OnClick = std::bind(BtnClick);

	layout->Children().Add(layout3);
	layout->Children().Add(layout2);
	layout2->Children().Add(button3);
	
	layout->Children().Add(button2);

	wind.SetContent(layout);

	wind.Show();

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}
