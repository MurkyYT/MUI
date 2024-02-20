#pragma once
#include "..\MUI\MUI.h"
#include "resource.h"
using namespace MUI;
class GridDemoWindow : public Window
{
public:
	GridDemoWindow();
private:
	CheckBox* checkBox;
	TextBlock* textBlock;
	ListView* listView;
	void WindowClose(/*UIComponent* sender, EventArgs_t e*/);
	void CheckBoxClick(UIComponent* sender,EventArgs_t e);
	void InitializeComponent()
	{
		Grid* grid = new Grid();
		this->SetBackroundColor(MUI::Color::PURPLE);
		this->m_StaticBacgkround = MUI::Color::PURPLE;
		this->m_StaticTextColor = MUI::Color::LIGHT_PURPLE;
		this->m_EditBacgkround = MUI::Color::LIGHT_PURPLE;
		this->m_ButtonBacgkround = MUI::Color::WHITE;
		this->m_ButtonTextColor = MUI::Color::DARK_PURPLE;
		grid->AddColumn(0, L"Auto");
		grid->AddColumn(0, L"*");
		grid->AddColumn(0, L"Auto");
		checkBox = new CheckBox(L"CheckBox", 100, 300, 70, 15);
		checkBox->SetVerticalAligment(Center);
		checkBox->OnClick = std::bind(&GridDemoWindow::CheckBoxClick, this, std::placeholders::_1, std::placeholders::_2);
		listView = new ListView(10, 10, 200, 200);
		listView->SetHorizontalAligment(Stretch);
		listView->SetVerticalAligment(Stretch);
		textBlock = new TextBlock(L"", 180, 0, 70, 15);
		textBlock->SetVerticalAligment(Center);
		textBlock->SetText(L"Checked");
		grid->AddItem(listView, 0, 1);
		grid->AddItem(checkBox, 0, 2);
		grid->AddItem(textBlock, 0, 0);
		this->OnClose = std::bind(&GridDemoWindow::WindowClose, this);
		this->Create(L"MUI Grid Demo", (DWORD)IDI_ICON1);
		this->SetGrid(grid);
		listView->Clear();
		HICON ico = LoadIcon(this->GetHINSTACE(), MAKEINTRESOURCE(IDI_ICON1));
		listView->AddIcon(ico);
		listView->AddColumn(L"Test", 100);
		listView->AddColumn(L"Test2", 100);
		ListItem itm(0, std::vector<std::wstring> {L"TEST1", L"TEST2"});
		listView->AddItem(&itm);
		ListItem itm2(0, std::vector<std::wstring> {L"TEST3", L"TEST4"});
		listView->AddItem(&itm2);
		checkBox->SetChecked(1);
	}
};
