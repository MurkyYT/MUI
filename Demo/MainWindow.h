#include <MUI.h>

#include "resource.h"

#include "StackLayoutTest1.h"
#include "StackLayoutTest2.h"
#include "StackLayoutTest3.h"
#include "StackLayoutTest4.h"

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
	void InitializeComponent()
	{
		SetTitle(L"MUI Demo Window");
		auto layout = std::make_shared<StackLayout>(Horizontal);
		auto layout2 = std::make_shared<StackLayout>(Vertical);
		layout->Children().Add(layout2);
		auto button = std::make_shared<Button>(L"Stack Layout Test 1");
		button->OnClick = std::bind(&MainWindow::OpenStack1, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Stack Layout Test 2");
		button->OnClick = std::bind(&MainWindow::OpenStack2, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Stack Layout Test 3");
		button->OnClick = std::bind(&MainWindow::OpenStack3, this);
		layout2->Children().Add(button);

		button = std::make_shared<Button>(L"Stack Layout Test 4");
		button->OnClick = std::bind(&MainWindow::OpenStack4, this);
		layout2->Children().Add(button);

		SetContent(layout);
	}

	std::shared_ptr<StackLayoutTest1> stackLayoutTest1;
	std::shared_ptr<StackLayoutTest2> stackLayoutTest2;
	std::shared_ptr<StackLayoutTest3> stackLayoutTest3;
	std::shared_ptr<StackLayoutTest4> stackLayoutTest4;
};