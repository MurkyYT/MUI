#include <MUI.h>

#include "resource.h"

#include "StackLayoutTest1.h"
#include "StackLayoutTest2.h"
#include "TreeViewExample.h"
#include "DarkThemeWindow.h"
#include "DisabledEnabledWindow.h"

using namespace mui;

class MainWindow : public Window
{
public:
	MainWindow();
private:
	void OpenStack1()
	{
		if (stackLayoutTest1)
		{
			stackLayoutTest1->Close();
			stackLayoutTest1 = NULL;
		}

		stackLayoutTest1 = std::make_shared<StackLayoutTest1>();
		stackLayoutTest1->Show();
	}
	void OpenTreeView()
	{
		if (treeViewExample)
		{
			treeViewExample->Close();
			treeViewExample = NULL;
		}

		treeViewExample = std::make_shared<TreeViewExample>();
		treeViewExample->Show();
	}
	void OpenStack2()
	{
		if (stackLayoutTest2)
		{
			stackLayoutTest2->Close();
			stackLayoutTest2 = NULL;
		}

		stackLayoutTest2 = std::make_shared<StackLayoutTest2>();
		stackLayoutTest2->Show();
	}

	void OpenDisabledEnabled()
	{
		if (disabledEnabled)
		{
			disabledEnabled->Close();
			disabledEnabled = NULL;
		}

		disabledEnabled = std::make_shared<DisabledEnabledWindow>();
		disabledEnabled->Show();
	}
	void OpenDarkTheme()
	{
		if (darkThemeWindow)
		{
			darkThemeWindow->Close();
			darkThemeWindow = NULL;
		}

		darkThemeWindow = std::make_shared<DarkThemeWindow>();
		darkThemeWindow->Show();
	}
	void InitializeComponent()
	{
		SetTitle(L"MUI Demo Window");
		auto layout = std::make_shared<StackLayout>(Horizontal);
		auto layout2 = std::make_shared<StackLayout>(Vertical);
		auto layout3 = std::make_shared<StackLayout>(Vertical);
		layout->Children().Add(layout2);
		layout->Children().Add(layout3);

		auto button = std::make_shared<Button>(L"Stack Layout Test 1");
		button->OnClick = std::bind(&MainWindow::OpenStack1, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Stack Layout Test 2");
		button->OnClick = std::bind(&MainWindow::OpenStack2, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Tree View Example");
		button->OnClick = std::bind(&MainWindow::OpenTreeView, this);
		layout3->Children().Add(button);

		button = std::make_shared<Button>(L"Disabled/Enabled elements");
		button->OnClick = std::bind(&MainWindow::OpenDisabledEnabled, this);
		layout3->Children().Add(button);

		button = std::make_shared<Button>(L"Dark Theme Window");
		button->OnClick = std::bind(&MainWindow::OpenDarkTheme, this);
		layout3->Children().Add(button);

		SetContent(layout);
	}

	std::shared_ptr<StackLayoutTest1> stackLayoutTest1;
	std::shared_ptr<TreeViewExample> treeViewExample;
	std::shared_ptr<StackLayoutTest2> stackLayoutTest2;
	std::shared_ptr<DisabledEnabledWindow> disabledEnabled;
	std::shared_ptr<DarkThemeWindow> darkThemeWindow;
};