#pragma once

#include <MUI.h>

using namespace mui;

class StackLayoutTest3 : public Window
{
public:
	StackLayoutTest3();
private:
	void InitializeComponent()
	{
		SetTitle(L"StackLayoutTest3");
		std::shared_ptr<StackLayout> layout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);
		std::shared_ptr<StackLayout> layout2 = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);

		auto button1 = std::make_shared<Button>(L"Test");
		auto button2 = std::make_shared<Button>(L"Test");
		auto button3 = std::make_shared<Button>(L"Test");
		auto button4 = std::make_shared<Button>(L"Test");

		layout2->Children().Add(button1);
		layout2->Children().Add(button2);
		layout2->Children().Add(button3);

		layout->Children().Add(layout2);

		layout->Children().Add(button4);

		SetContent(layout);
	}
};

