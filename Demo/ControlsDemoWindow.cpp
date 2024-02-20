#include "ControlsDemoWindow.h"
ControlsDemoWindow::ControlsDemoWindow()
{
    InitializeComponent();
}
void ControlsDemoWindow::ButtonClick()
{
    std::wstring res = textBox->GetText();
    MessageBox(NULL, res.c_str(), L"TextBox text is", MB_OK);
}
void ControlsDemoWindow::RadioGroupChanged()
{
    int index = group->CurrentRadioButton();
    wchar_t buffer[256];
    wsprintfW(buffer, L"Radio Index: %d", index);
    radioBlock->SetText(buffer);
}
