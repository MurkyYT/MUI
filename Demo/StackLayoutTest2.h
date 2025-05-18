#pragma once

#include <MUI.h>

using namespace mui;

class StackLayoutTest2 : public Window
{
public:
	StackLayoutTest2();
private:
	void BtnClick()
	{
		std::shared_ptr<CheckBox> button = std::make_shared<CheckBox>(L"Test checkbox");
		button->SetTextColor(RGB(230, 230, 230));
		layout3->Children().Add(button);
		layout->Children().Remove(button2);
	}
	void InitializeComponent()
	{
		SetTitle(L"StackLayoutTest2");
		std::shared_ptr<StackLayout> layout2 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		layout3 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		layout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);;
		button2 = std::make_shared<Button>(L"Test button 2");;

		layout->SetBackgroundColor(RGB(0, 0, 0));

		std::shared_ptr<Button> button3 = std::make_shared<Button>(L"Test button 3");
		for (int i = 0; i < 20; i++) {
			std::shared_ptr<Button> button4 = std::make_shared<Button>((std::wstring(L"Test button (") + std::to_wstring(i + 1) + L")").c_str());
			button4->OnClick = std::bind(&StackLayoutTest2::BtnClick, this);
			layout2->Children().Add(button4);
		}

		button2->OnClick = std::bind(&StackLayoutTest2::BtnClick, this);

		layout->Children().Add(layout3);
		layout->Children().Add(layout2);
		layout2->Children().Add(button3);

		layout->Children().Add(button2);

		SetContent(layout);
	}

	std::shared_ptr<StackLayout> layout3;
	std::shared_ptr<StackLayout> layout;
	std::shared_ptr<Button> button2;
};

