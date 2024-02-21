#include "MainWindow.h"
MainWindow::MainWindow()
{
    InitializeComponent();
}
void MainWindow::OpenGridExample()
{
    if (!gridDemo || gridDemo->IsHidden())
    {
        if (gridDemo)
            delete gridDemo;
        gridDemo = new GridDemoWindow();
        gridDemo->Show();
    }
    else
        gridDemo->Activate();
}
void MainWindow::OpenControlsDemo()
{

	if (!controlsDemo || controlsDemo->IsHidden())
	{
        if(controlsDemo)
            delete controlsDemo;
		controlsDemo = new ControlsDemoWindow();
		controlsDemo->Show();
	}
	else
		controlsDemo->Activate();
}
void MainWindow::ShowAbout()
{
    MessageBoxW(this->GetHWND(),L"MUI - UI Framework built on top of win32 API", L"MUI", MB_OK|MB_ICONINFORMATION);
}
void MainWindow::Exit()
{
    PostQuitMessage(0);
}