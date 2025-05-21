#pragma once

#include <MUI.h>

using namespace mui;
class GridWindow : public Window
{
public:
	GridWindow();
private:
	void InitializeComponent()
	{
		SetTitle(L"MUI Grid Demo");
		auto grid = std::make_shared<Grid>();

		auto button = std::make_shared<Button>(L"Test");

		grid->AddRow(GridLength::Auto());
		grid->AddRow(GridLength::Star());
		grid->AddRow(GridLength::Auto());

		grid->AddChild(button);
		grid->SetRow(button, 0);
		auto lv = std::make_shared<ListView>();
		grid->AddChild(lv);
		lv->AddColumn(L"Hello!");
		lv->AddItem(std::make_shared<ListItem>(std::vector<std::wstring>{L"Test"}, (HICON)NULL));
		button = std::make_shared<Button>(L"Test2");

		grid->SetRow(lv, 1);
		grid->AddChild(button);
		grid->SetRow(button, 2);

		SetContent(grid);
	}
};

