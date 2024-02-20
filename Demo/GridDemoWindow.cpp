#include "GridDemoWindow.h"
GridDemoWindow::GridDemoWindow()
{
	InitializeComponent();
}
void GridDemoWindow::WindowClose()
{
	OutputDebugString(L"Window is closed!\n");
}
void GridDemoWindow::CheckBoxClick(UIComponent* sender,EventArgs_t e)
{
    if (checkBox->IsChecked()) {
        //button.SetStyle(button.GetStyle() | BS_OWNERDRAW);
        textBlock->SetText(L"Checked");
        listView->Show();
    }
    else
    {
        //button.SetStyle(button.GetStyle() & ~BS_OWNERDRAW);
        textBlock->SetText(L"UnChecked");
        listView->Hide();
    }
}
