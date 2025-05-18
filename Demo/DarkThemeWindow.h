#pragma once

#include <MUI.h>

#include "resource.h"

using namespace mui;

class DarkThemeWindow : public Window
{
public:
	DarkThemeWindow();
private:
	void RemoveItem()
	{
		if (listView->ItemCount())
			listView->RemoveItemByIndex(0);

		label->SetText(L"List view example: " + std::to_wstring(listView->ItemCount()));
	}
	void OnKeyDown(void* element, EventArgs_t args)
	{
		if (args.wParam == VK_DELETE)
			RemoveItem();
	}
	void AddItem()
	{
		auto itm = std::make_shared<ListItem>(std::vector<std::wstring>{ entry->GetText() }, checkBox->IsChecked() ? LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)) : NULL);
		listView->AddItem(itm);
		label->SetText(L"List view example: " + std::to_wstring(listView->ItemCount()));

		entry->SetText(L"");
	}
	void InitializeComponent()
	{
		SetTitle(L"Dark Theme Window");
		this->KeyDown = std::bind(&DarkThemeWindow::OnKeyDown, this, std::placeholders::_1, std::placeholders::_2);
		auto layout = std::make_shared<StackLayout>(Vertical);
		auto button = std::make_shared<Button>(L"Add item");
		auto button2 = std::make_shared<Button>(L"Remove first item");
		checkBox = std::make_shared<CheckBox>(L"Add item with icon");
		label = std::make_shared<Label>(L"List view example: 0");
		listView = std::make_shared<ListView>();
		entry = std::make_shared<Entry>(L"");
		entry->SetPlaceholder(L"Enter item text");
		entry->Completed = std::bind(&DarkThemeWindow::AddItem, this);

		checkBox->SetHorizontalAligment(Center);
		checkBox->SetChecked(TRUE);

		listView->AddColumn(L"Values");

		listView->SetBackgroundColor(RGB(30, 30, 30));
		listView->SetHoverColor(RGB(45, 45, 45));
		listView->SetTextColor(RGB(230, 230, 230));
		listView->SetSeparatorColor(RGB(128, 128, 128));

		entry->SetBackgroundColor(RGB(30, 30, 30));
		entry->SetTextColor(RGB(230, 230, 230));

		button->SetBackgroundColor(RGB(30, 30, 30));
		button->SetRegularColor(RGB(30, 30, 30));
		button->SetHoverColor(RGB(45, 45, 45));
		button->SetTextColor(RGB(230, 230, 230));
		button->SetBorderColor(RGB(128, 128, 128));
		button->SetPressedColor(RGB(65, 65, 65));

		button2->SetRegularColor(RGB(30, 30, 30));
		button2->SetBackgroundColor(RGB(30, 30, 30));
		button2->SetHoverColor(RGB(45, 45, 45));
		button2->SetTextColor(RGB(230, 230, 230));
		button2->SetBorderColor(RGB(128, 128, 128));
		button2->SetPressedColor(RGB(65, 65, 65));

		checkBox->SetTextColor(RGB(230, 230, 230));
		checkBox->SetBackgroundColor(RGB(30, 30, 30));

		label->SetBackgroundColor(RGB(30, 30, 30));
		label->SetTextColor(RGB(230, 230, 230));

		layout->SetBackgroundColor(RGB(30, 30, 30));

		layout->Children().Add(label);
		layout->Children().Add(listView);

		button->OnClick = std::bind(&DarkThemeWindow::AddItem, this);
		button2->OnClick = std::bind(&DarkThemeWindow::RemoveItem, this);

		layout->Children().Add(checkBox);
		layout->Children().Add(entry);
		layout->Children().Add(button);
		layout->Children().Add(button2);

		SetContent(layout);

		label->SetTextAligment(Center);
	}

	std::shared_ptr<ListView> listView;
	std::shared_ptr<Label> label;
	std::shared_ptr<CheckBox> checkBox;
	std::shared_ptr<Entry> entry;
};

