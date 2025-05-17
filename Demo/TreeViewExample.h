#pragma once

#include <MUI.h>
#include "resource.h"

using namespace mui;

class TreeViewExample : public Window
{
public:
	TreeViewExample();
private:
	void DoubleClick()
	{
		if(tv->Count() > 1)
			tv->RemoveChild(tv->GetSelectedItem());
		tv->AddChild(std::make_shared<TreeViewItem>(L"Test", LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1))));
		tv->AddChild(std::make_shared<TreeViewItem>(L"Test", LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1))));
	}
	void InitializeComponent()
	{
		SetTitle(L"Tree View Example");

		tv = std::make_shared<TreeView>();
		child = std::make_shared<TreeViewItem>(L"Test", LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
		auto child2 = std::make_shared<TreeViewItem>(L"Test2", LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
		child3 = std::make_shared<TreeViewItem>(L"Test3", (HICON)NULL);
		tv->AddChild(child);
		child->AddChild(child2);
		child->AddChild(child3);

		tv->DoubleClick = std::bind(&TreeViewExample::DoubleClick, this);

		tv->SetBackgroundColor(RGB(30, 30, 30));
		tv->SetTextColor(RGB(230, 230, 230));
		tv->SetExpandButtonColor(RGB(214, 214, 214));
		tv->SetSelectedColor(RGB(56, 56, 56));

		SetContent(tv);
	}

	std::shared_ptr<TreeView> tv;
	std::shared_ptr<TreeViewItem> child3;
	std::shared_ptr<TreeViewItem> child;
};

