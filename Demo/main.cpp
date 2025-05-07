#include <MUI.h>
#include <string>
using namespace mui;

std::shared_ptr<StackLayout> layout3 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
std::shared_ptr<StackLayout> layout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);
std::shared_ptr<Button> button2 = std::make_shared<Button>(L"Test button 2");

void BtnClick()
{
	std::shared_ptr<CheckBox> button = std::make_shared<CheckBox>(L"Test checkbox");
	layout3->Children().Add(button);
	layout->Children().Remove(button2);

	if(layout3->Children().Count() != 1)
		button->SetText(std::wstring(L"CheckBox " + std::to_wstring(layout3->Children().Count())).c_str());
}

INT WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Window wind(L"Test");

	std::shared_ptr<StackLayout> layout2 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);

	std::shared_ptr<Button> button3 = std::make_shared<Button>(L"Test button 3");

	for (int i = 0; i < 20; i++) {
		std::shared_ptr<Button> button4 = std::make_shared<Button>((std::wstring(L"Test button (") + std::to_wstring(i + 1) + L")").c_str());
		button4->OnClick = std::bind(BtnClick);
		layout2->Children().Add(button4);
		wind.SetTitle((wind.GetTitle() + L" " + std::to_wstring(i + 1)).c_str());
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
