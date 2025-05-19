#pragma region GENERATED_INCLUDES
#include <MUI.h>

#include "resource.h"

using namespace mui;
#pragma endregion

#include "StackLayoutTest1.h"
#include "StackLayoutTest2.h"
#include "TreeViewExample.h"
#include "DarkThemeWindow.h"
#include "DisabledEnabledWindow.h"
#include "SectionTest.h"
#include "GridWindow.h"

class MainWindow : public Window
{
public:
	MainWindow();
private:
	std::shared_ptr<StackLayoutTest1> stackLayoutTest1;
	std::shared_ptr<StackLayoutTest2> stackLayoutTest2;
	std::shared_ptr<SectionTest> sectionTest;
	std::shared_ptr<TreeViewExample> treeViewExample;
	std::shared_ptr<DisabledEnabledWindow> disabledEnabled;
	std::shared_ptr<DarkThemeWindow> darkThemeWindow;
	std::shared_ptr<GridWindow> gridWindow;

#pragma region GENERATED_CLASS
private:
	void OpenStack1();
	void OpenStack2();
	void OpenSectionTest();
	void OpenTreeView();
	void OpenDisabledEnabled();
	void OpenDarkTheme();
	void OpenGrid();

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

		button = std::make_shared<Button>(L"Section Test");
		button->OnClick = std::bind(&MainWindow::OpenSectionTest, this);
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

		button = std::make_shared<Button>(L"Grid Window");
		button->OnClick = std::bind(&MainWindow::OpenGrid, this);
		layout3->Children().Add(button);

		SetContent(layout);
	}
#pragma endregion
};