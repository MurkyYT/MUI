#include "ControlsDemoWindow.h"
ControlsDemoWindow::ControlsDemoWindow()
{
    InitializeComponent();
}
void ControlsDemoWindow::ButtonClick()
{
    std::wstring res = textBox->GetText();
    /*textBox->EnableHorizontalScrollbar(!textBox->HorizontalScrollbarEnabled());
    textBox->EnableVerticalScrollbar(!textBox->VerticalScrollbarEnabled());*/
    MessageBox(NULL, res.c_str(), L"TextBox text is", MB_OK);
}
void ControlsDemoWindow::ContextMenuOpen()
{
    ContextMenu* menu = new ContextMenu();
    Menu* test = new Menu(L"Test");
    Menu* test2 = new Menu(L"Test2");
    menu->Add(test);
    menu->Add(new Separator());
    menu->Add(test2);
    menu->Add(new Separator());
    menu->Add(test);
    menu->Add(new Separator());
    menu->Add(test2);
    menu->Add(new Separator());
    menu->Add(test);
    menu->Add(new Separator());
    menu->Add(test2);
    menu->Add(new Separator());
    menu->Add(test);
    menu->Add(new Separator());
    menu->Add(test2);
    menu->Open();
}
void ControlsDemoWindow::RadioGroupChanged()
{
    int index = group->CurrentRadioButton();
    wchar_t buffer[256];
    wsprintfW(buffer, L"Radio Index: %d", index);
    radioBlock->SetText(buffer);
}
