#include <MUI.h>

#include "resource.h"

using namespace mui;

class MainWindow : public Window
{
public:
	MainWindow();
private:
	void RemoveItem()
	{
		if(listView->ItemCount())
			listView->RemoveItemByIndex(0);

		label->SetText(L"List view example: " + std::to_wstring(listView->ItemCount()));
	}
	void AddItem()
	{
		auto itm = std::make_shared<ListItem>(std::vector<std::wstring>{ L"Hello!", L"Test" }, listView->ItemCount() % 5 == 0 ? LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)) : NULL);
		listView->AddItem(itm);
		label->SetText(L"List view example: " + std::to_wstring(listView->ItemCount()));
	}
	void InitializeComponent()
	{
		SetTitle(L"MUI Demo Window");
		auto layout = std::make_shared<StackLayout>(Vertical);
		auto button = std::make_shared<Button>(L"Add item");
		auto button2 = std::make_shared<Button>(L"Remove first item");
		label = std::make_shared<Label>(L"List view example: 1");
		listView = std::make_shared<ListView>();

		auto itm = std::make_shared<ListItem>(std::vector<std::wstring>{ L"Hello!", L"Test" }, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));

		listView->AddColumn(L"Test1");
		listView->AddColumn(L"Test2");

		listView->AddItem(itm);

		layout->Children().Add(label);
		layout->Children().Add(listView);

		button->OnClick = std::bind(&MainWindow::AddItem, this);
		button2->OnClick = std::bind(&MainWindow::RemoveItem, this);

		layout->Children().Add(button);
		layout->Children().Add(button2);

		SetContent(layout);

		label->SetTextAligment(Center);
	}

	std::shared_ptr<ListView> listView;
	std::shared_ptr<Label> label;
};