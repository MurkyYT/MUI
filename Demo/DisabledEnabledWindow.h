#pragma once

#include <MUI.h>

using namespace mui;

class DisabledEnabledWindow : public Window
{
public:
	DisabledEnabledWindow();
private:
	void InitializeComponent()
	{
		SetTitle(L"Disabled/Enabled elements");
		auto mainLayout = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		auto listView = std::make_shared<ListView>();
		auto item = std::make_shared<ListItem>(std::vector<std::wstring>{L"Enabled!"});
		listView->AddColumn(L"Enabled!");
		listView->AddItem(item);

		mainLayout->Children().Add(listView);

		item = std::make_shared<ListItem>(std::vector<std::wstring>{L"Disabled!"});
		listView = std::make_shared<ListView>();
		listView->AddColumn(L"Disabled!");
		listView->AddItem(item);
		listView->SetEnabled(FALSE);

		mainLayout->Children().Add(listView);

		auto horizontalLayout = std::make_shared<StackLayout>(StackLayoutOrientation::Horizontal);
		auto layout = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		auto button = std::make_shared<Button>(L"Enabled!");
		horizontalLayout->Children().Add(layout);
		layout->Children().Add(button);

		button = std::make_shared<Button>(L"Disabled!");
		button->SetEnabled(FALSE);
		layout->Children().Add(button);

		layout = std::make_shared<StackLayout>(StackLayoutOrientation::Vertical);
		auto checkBox = std::make_shared<CheckBox>(L"Enabled!");
		horizontalLayout->Children().Add(layout);
		layout->Children().Add(checkBox);

		checkBox = std::make_shared<CheckBox>(L"Disabled!");
		checkBox->SetEnabled(FALSE);
		layout->Children().Add(checkBox);
		checkBox = std::make_shared<CheckBox>(L"Disabled!");
		checkBox->SetEnabled(FALSE);
		checkBox->SetChecked(TRUE);
		layout->Children().Add(checkBox);

		mainLayout->Children().Add(horizontalLayout);

		SetContent(mainLayout);
	}
};

