#pragma once
#include "..\MUI\MUI.h"
#include "GridDemoWindow.h"
#include "ControlsDemoWindow.h"
#include "resource.h"
using namespace MUI;
class MainWindow : public Window
{
public:
	MainWindow();
private:
	void Exit();
	void OpenGridExample();
	void OpenControlsDemo();
	void ShowAbout();
	void ControlsDemoClosed();
	void GridDemoClosed();
	GridDemoWindow* gridDemo;
	ControlsDemoWindow* controlsDemo;
	void InitializeComponent()
	{
		MenuBar* dockMenu = new MenuBar();
		Menu* gridExampleMenu = new Menu(L"Grid Example");
		Menu* controlsDemoMenu = new Menu(L"Controls Example");
		Menu* menu = new Menu(L"File");
		Menu* exitMenu = new Menu(L"Exit");
		Menu* aboutMenu = new Menu(L"About");
		Menu* examples = new Menu(L"Examples");
		exitMenu->OnClick = std::bind(&MainWindow::Exit,this);
		aboutMenu->OnClick = std::bind(&MainWindow::ShowAbout, this);
		gridExampleMenu->OnClick = std::bind(&MainWindow::OpenGridExample,this);
		controlsDemoMenu->OnClick = std::bind(&MainWindow::OpenControlsDemo, this);
		examples->Add(gridExampleMenu);
		examples->Add(controlsDemoMenu);
		menu->Add(aboutMenu);
		menu->Add(new Separator());
		menu->Add(exitMenu);
		dockMenu->Add(menu);
		dockMenu->Add(examples);
		this->Create(L"MUI Demo", (DWORD)IDI_ICON1);
		MUI::Image* img = new MUI::Image(0, 0, 0, 0);
		this->AddComponent(img);
		img->SetImage((HBITMAP)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 60, 60, NULL));
		img->SetHorizontalAligment(Stretch);
		img->SetVerticalAligment(Stretch);
		this->SetMenuBar(dockMenu);
		this->MinSize.x = 500;
		this->MinSize.y = 500;
		this->MaxSize = this->MinSize;
	}
};

