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

		grid->AddColumn(GridLength::Auto());
		grid->AddColumn(GridLength::Star());
		grid->AddColumn(GridLength::Auto());

		grid->AddChild(button, 0, 0);
		auto lv = std::make_shared<ListView>();
		lv->AddColumn(L"Hello!");
		lv->AddItem(std::make_shared<ListItem>(std::vector<std::wstring>{L"Test!"}, (HICON)NULL));
		button = std::make_shared<Button>(L"Test2");

		grid->AddChild(lv, 0, 1);
		grid->AddChild(button, 0, 2);

		SetContent(grid);
	}
};

