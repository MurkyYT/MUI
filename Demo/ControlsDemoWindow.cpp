#include "ControlsDemoWindow.h"
ControlsDemoWindow::ControlsDemoWindow()
{
    InitializeComponent();
    menu = new ContextMenu();
    Menu* test = new Menu(L"Test");
    Menu* test2 = new Menu(L"Test2");
    test3 = new Menu(L"Test3");
    test->OnClick = std::bind(&ControlsDemoWindow::ButtonClick, this);
    test->Add(test2);
    test2->SetChecked(TRUE);
    test2->OnClick = std::bind(&ControlsDemoWindow::Test2Clicked,this, std::placeholders::_1);
    menu->Add(test);
    menu->Add(new Separator());
    menu->Add(test3);
}
void ControlsDemoWindow::Test2Clicked(UIComponent* sender)
{
    BOOL checked = ((Menu*)sender)->IsChecked();
    ((Menu*)sender)->SetChecked(!checked);
    test3->SetEnabled(!checked);
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
    menu->Open();
}
void ControlsDemoWindow::RadioGroupChanged()
{
    int index = group->CurrentRadioButton();
    wchar_t buffer[256];
    wsprintfW(buffer, L"Radio Index: %d", index);
    radioBlock->SetText(buffer);
}
