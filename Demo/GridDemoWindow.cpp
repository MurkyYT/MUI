#include "GridDemoWindow.h"
GridDemoWindow::GridDemoWindow()
{
	InitializeComponent();
}
void GridDemoWindow::WindowClose()
{
	OutputDebugString(L"Window is closed!\n");
}
void GridDemoWindow::CheckBoxClick()
{
    if (checkBox->IsChecked()) {
        textBlock->SetText(L"Checked");
        listView->Show();
    }
    else
    {
        textBlock->SetText(L"UnChecked");
        listView->Hide();
    }
}
