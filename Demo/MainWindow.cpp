#include "MainWindow.h"
MainWindow::MainWindow()
{
    InitializeComponent();
}
void MainWindow::OpenGridExample()
{
    if (!gridDemo || gridDemo->IsHidden())
    {
        gridDemo = std::shared_ptr<GridDemoWindow>(new GridDemoWindow());
        gridDemo->Show();
    }
    else
        gridDemo->Activate();
}
void MainWindow::OpenControlsDemo()
{

	if (!controlsDemo || controlsDemo->IsHidden())
	{
		controlsDemo = std::shared_ptr<ControlsDemoWindow>(new ControlsDemoWindow());
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