#include <MUI.h>

#include "resource.h"

#include "StackLayoutTest1.h"
#include "StackLayoutTest3.h"
#include "StackLayoutTest4.h"
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
	void OpenStack3()
	{
		if (stackLayoutTest3)
		{
			stackLayoutTest3->Close();
			stackLayoutTest3 = NULL;
		}

		stackLayoutTest3 = std::make_shared<StackLayoutTest3>();
		stackLayoutTest3->Show();
	}
	void OpenStack4()
	{
		if (stackLayoutTest4)
		{
			stackLayoutTest4->Close();
			stackLayoutTest4 = NULL;
		}

		stackLayoutTest4 = std::make_shared<StackLayoutTest4>();
		stackLayoutTest4->Show();
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

		button = std::make_shared<Button>(L"Stack Layout Test 3");
		button->OnClick = std::bind(&MainWindow::OpenStack3, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Stack Layout Test 4");
		button->OnClick = std::bind(&MainWindow::OpenStack4, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Disabled/Enabled elements");
		button->OnClick = std::bind(&MainWindow::OpenDisabledEnabled, this);
		layout3->Children().Add(button);

		button = std::make_shared<Button>(L"Dark Theme Window");
		button->OnClick = std::bind(&MainWindow::OpenDarkTheme, this);
		layout3->Children().Add(button);

		SetContent(layout);
	}

	std::shared_ptr<StackLayoutTest1> stackLayoutTest1;
	std::shared_ptr<StackLayoutTest3> stackLayoutTest3;
	std::shared_ptr<StackLayoutTest4> stackLayoutTest4;
	std::shared_ptr<DisabledEnabledWindow> disabledEnabled;
	std::shared_ptr<DarkThemeWindow> darkThemeWindow;
};