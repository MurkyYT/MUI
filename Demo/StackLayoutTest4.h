#pragma once

#include <MUI.h>

using namespace mui;

class StackLayoutTest4 : public Window
{
public:
	StackLayoutTest4();
private:
	void BtnClick()
	{
		std::shared_ptr<CheckBox> button = std::make_shared<CheckBox>(L"Test checkbox");
		layout3->Children().Add(button);
		layout->Children().Remove(button2);
	}
	void InitializeComponent()
	{
		SetTitle(L"StackLayoutTest4");
		std::shared_ptr<StackLayout> layout2 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		layout3 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		layout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);;
		button2 = std::make_shared<Button>(L"Test button 2");;


		std::shared_ptr<Button> button3 = std::make_shared<Button>(L"Test button 3");
		for (int i = 0; i < 20; i++) {
			std::shared_ptr<Button> button4 = std::make_shared<Button>((std::wstring(L"Test button (") + std::to_wstring(i + 1) + L")").c_str());
			button4->OnClick = std::bind(&StackLayoutTest4::BtnClick, this);
			layout2->Children().Add(button4);
		}

		button2->OnClick = std::bind(&StackLayoutTest4::BtnClick, this);

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

