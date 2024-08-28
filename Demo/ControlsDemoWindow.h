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
	Menu* test3;
	ContextMenu* menu;
	void RadioGroupChanged();
	void Test2Clicked(UIComponent* sender);
	void ButtonClick();
	void ContextMenuOpen();
	void InitializeComponent()
	{
		this->Create(L"MUI Controls Demo", (DWORD)IDI_ICON1);
		textBox = new TextBox(L"Test-TextBox", FALSE, 0, 0, 100, 100);
		/*
		* Add vertical scroll bar.
		* If before adding to window it shows correctly, if after, only scroll wheel works(could be a bug ? )
		*/
		textBox->SetStyle(textBox->GetStyle() | WS_VSCROLL);
		Button* button = new Button(L"Input-Box text", FALSE, 0, 100, 100, 20);
		Button* button4 = new Button(L"Open contextmenu", FALSE, 0, 200, 200, 20);
		RadioButton* button1 = new RadioButton(L"Fullscreen", 130, 90, 90, 15);
		RadioButton* button2 = new RadioButton(L"Windowed Fullscreen", 130, 110, 140, 15);
		RadioButton* button3 = new RadioButton(L"Windowed", 130, 130, 140, 15);
		radioBlock = new TextBlock(L"Radio Index: -1", 120, 50, 100, 15);
		group = new RadioGroup(L"Window Mode", 120, 70, 209, 85);
		this->AddComponent(button1);
		this->AddComponent(button2);
		this->AddComponent(button3);
		this->AddComponent(button);
		this->AddComponent(button4);
		this->AddComponent(textBox);
		this->AddComponent(group);
		this->AddComponent(radioBlock);
		group->AddRadioButton(button1);
		group->AddRadioButton(button2);
		group->AddRadioButton(button3);
		button->OnClick = std::bind(&ControlsDemoWindow::ButtonClick, this);
		group->OnChange = std::bind(&ControlsDemoWindow::RadioGroupChanged, this);
		button4->OnClick = std::bind(&ControlsDemoWindow::ContextMenuOpen, this);
	}
};

