#pragma once
#include <MUI.h>

using namespace mui;
class SectionTest : public Window
{
public:
	SectionTest() { InitializeComponent(); }
private:
	void InitializeComponent()
	{
		SetTitle(L"Section Test");

		auto section = std::make_shared<Section>(L"Test Section");

		auto button = std::make_shared<Button>(L"test button");

		auto layout = std::make_shared<StackLayout>(Horizontal);

		layout->Children().Add(button);

		button = std::make_shared<Button>(L"test button");

		layout->Children().Add(button);

		section->SetHorizontalAligment(Start);
		section->SetVerticalAligment(Center);

		section->SetContent(layout);

		SetContent(section);
		layout->SetBackgroundColor(RGB(30, 30, 30));
		section->SetBackgroundColor(RGB(30, 30, 30));
		section->SetTextColor(RGB(230, 230, 230));
		section->SetExpandButtonColor(RGB(219, 219, 219));
		SetBackgroundColor(RGB(30, 30, 30));
	}
};

