#include "..\MUI\MUI.h"
#include "resource.h"

using namespace MUI;
void CheckBoxClick();
void ButtonClick();
void RadioGroupChanged();
void WindowClose();
TextBox box(L"Test-TextBox", FALSE, 0, 0, 100, 100);
Button button(L"Input-Box text",FALSE, 0, 100, 100, 20);
CheckBox chkbox(L"CheckBox", 100, 300, 70, 15);
TextBlock block(L"", 180, 0, 70, 15);
TextBlock radioBlock(L"Radio Index: -1", 120, 50, 100, 15);
RadioGroup group(L"Window Mode", 120, 70, 209, 85);
RadioButton button1(L"Fullscreen", 130, 90, 90, 15);
RadioButton button2(L"Windowed Fullscreen", 130, 110, 140, 15);
RadioButton button3(L"Windowed", 130, 130, 140, 15);
ListView lstView(10, 10, 200, 200);
Grid grid;
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    
    Window window(hInstance);
    window.SubscribeToOnClose(WindowClose);
    button.SubscribeToOnClick(ButtonClick);
    chkbox.SubscribeToOnClick(CheckBoxClick);
    group.SubscribeToOnChange(RadioGroupChanged);
    group.AddRadioButton(&button1);
    group.AddRadioButton(&button2);
    group.AddRadioButton(&button3);
    /*
    * Add vertical scroll bar.
    * If before window creation it shows correctly, if after, only scroll wheel works(could be a bug ? )
    */ 
    box.SetStyle(box.GetStyle() | WS_VSCROLL);
   /* window.SetBackroundColor(Color::PURPLE);
    window.m_StaticBacgkround = Color::PURPLE;
    window.m_StaticTextColor = Color::LIGHT_PURPLE;
    window.m_EditBacgkround = Color::LIGHT_PURPLE;
    window.m_ButtonBacgkround = Color::WHITE;
    window.m_ButtonTextColor = Color::DARK_PURPLE;*/
    if (!window.Create(L"MUI Demo",(DWORD)IDI_ICON1)) {
        return 0;
    }
    chkbox.SetChecked(1);

    std::vector<UIComponent*> comps =
    {
        &button,
        &group,
        &button1,
        &button2,
        &button3,
        &box,
        &radioBlock,
        &chkbox,
        &block
    };
    //window.AddComponents(comps);
   /* window.AddComponent(&chkbox);
    chkbox.SetHorizontalAligment(Center);
    chkbox.SetVerticalAligment(Top);
    chkbox.SetChecked(TRUE);
    window.AddComponent(&lstView);*/
    grid.AddColumn(100, L"*");
    grid.AddColumn(150, L"Auto");
    grid.AddRow(100, L"*");
    grid.AddRow(150, L"Auto");
    lstView.SetHorizontalAligment(Stretch);
    lstView.SetVerticalAligment(Stretch);
    block.SetVerticalAligment(Center);
    chkbox.SetVerticalAligment(Center);
    grid.AddItem(&lstView, 1,1);
    grid.AddItem(&chkbox, 1, 2);
    grid.AddItem(&block, 1, 0);
    window.SetGrid(&grid);
    window.Show(SW_SHOW);
    //lstView.Hide();
    HICON ico = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    lstView.AddIcon(ico);
    lstView.AddColumn(L"Test", 100);
    lstView.AddColumn(L"Test2", 100);
    ListItem itm(0, std::vector<std::wstring> {L"TEST1", L"TEST2"});
    lstView.AddItem(&itm);
    ListItem itm2(0, std::vector<std::wstring> {L"TEST3", L"TEST4"});
    lstView.AddItem(&itm2);

   /* window.MinSize.x = 500;
    window.MinSize.y = 500;
    window.MaxSize = window.MinSize;*/

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
void ButtonClick()
{
    std::wstring res = box.GetText();
    //chkbox.SetChecked(!chkbox.IsChecked());
    MessageBox(NULL, res.c_str(), L"TextBox text is", MB_OK);
}
void CheckBoxClick()
{
    if (chkbox.IsChecked()) {
        //button.SetStyle(button.GetStyle() | BS_OWNERDRAW);
        block.SetText(L"Checked");
        lstView.Show();
    }
    else
    {
        //button.SetStyle(button.GetStyle() & ~BS_OWNERDRAW);
        block.SetText(L"UnChecked");
        lstView.Hide();
    }
}
void RadioGroupChanged()
{
    int index = group.CurrentRadioButton();
    wchar_t buffer[256];
    wsprintfW(buffer, L"Radio Index: %d", index);
    radioBlock.SetText(buffer);
}
void WindowClose()
{
    OutputDebugString(L"Window is closed!\n");
}