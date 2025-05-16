#pragma once

#include <MUI.h>

using namespace mui;

class StackLayoutTest1 : public Window
{
public:
	StackLayoutTest1();
private:
	void InitializeComponent()
	{
		SetTitle(L"StackLayoutTest1");
		std::shared_ptr<StackLayout> layout = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);


		std::shared_ptr<Button> button = std::make_shared<Button>(L"Test");

		layout->Children().Add(button);

		this->SetContent(layout);
	}
};

