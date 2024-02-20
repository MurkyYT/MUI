#pragma once
#include "..\MUI\MUI.h"
#include "resource.h"
using namespace MUI;
class ControlsDemoWindow : public Window
{
public:
	ControlsDemoWindow();
private:
	RadioGroup* group;
	TextBlock* radioBlock;
	TextBox* textBox;
	void RadioGroupChanged();
	void ButtonClick();
	void InitializeComponent()
	{
		textBox = new TextBox(L"Test-TextBox", FALSE, 0, 0, 100, 100);
		/*
		* Add vertical scroll bar.
		* If before window creation it shows correctly, if after, only scroll wheel works(could be a bug ? )
		*/
		textBox->SetStyle(textBox->GetStyle() | WS_VSCROLL);
		Button* button = new Button(L"Input-Box text",FALSE, 0, 100, 100, 20);
		radioBlock = new TextBlock(L"Radio Index: -1", 120, 50, 100, 15);
		group = new RadioGroup(L"Window Mode", 120, 70, 209, 85);
		RadioButton* button1 = new RadioButton(L"Fullscreen", 130, 90, 90, 15);
		RadioButton* button2 = new RadioButton(L"Windowed Fullscreen", 130, 110, 140, 15);
		RadioButton* button3 = new RadioButton(L"Windowed", 130, 130, 140, 15);
		group->AddRadioButton(button1);
		group->AddRadioButton(button2);
		group->AddRadioButton(button3);
		button->OnClick = std::bind(&ControlsDemoWindow::ButtonClick, this);
		group->OnChange = std::bind(&ControlsDemoWindow::RadioGroupChanged, this);
		this->Create(L"MUI Controls Demo", (DWORD)IDI_ICON1);
		this->AddComponent(button1);
		this->AddComponent(button2);
		this->AddComponent(button3);
		this->AddComponent(button);
		this->AddComponent(textBox);
		this->AddComponent(group);
		this->AddComponent(radioBlock);
	}
};

