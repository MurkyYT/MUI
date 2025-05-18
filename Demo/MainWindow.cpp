#include "MainWindow.h"

MainWindow::MainWindow() 
{
	InitializeComponent();
}

void MainWindow::OpenStack1()
{
	if (stackLayoutTest1)
	{
		stackLayoutTest1->Close();
		stackLayoutTest1 = NULL;
	}

	stackLayoutTest1 = std::make_shared<StackLayoutTest1>();
	stackLayoutTest1->Show();
}

void MainWindow::OpenTreeView()
{
	if (treeViewExample)
	{
		treeViewExample->Close();
		treeViewExample = NULL;
	}

	treeViewExample = std::make_shared<TreeViewExample>();
	treeViewExample->Show();
}

void MainWindow::OpenStack2()
{
	if (stackLayoutTest2)
	{
		stackLayoutTest2->Close();
		stackLayoutTest2 = NULL;
	}

	stackLayoutTest2 = std::make_shared<StackLayoutTest2>();
	stackLayoutTest2->Show();
}

void MainWindow::OpenDisabledEnabled()
{
	if (disabledEnabled)
	{
		disabledEnabled->Close();
		disabledEnabled = NULL;
	}

	disabledEnabled = std::make_shared<DisabledEnabledWindow>();
	disabledEnabled->Show();
}

void MainWindow::OpenDarkTheme()
{
	if (darkThemeWindow)
	{
		darkThemeWindow->Close();
		darkThemeWindow = NULL;
	}

	darkThemeWindow = std::make_shared<DarkThemeWindow>();
	darkThemeWindow->Show();
}

void MainWindow::OpenSectionTest()
{
	if (sectionTest)
	{
		sectionTest->Close();
		sectionTest = NULL;
	}

	sectionTest = std::make_shared<SectionTest>();
	sectionTest->Show();
}