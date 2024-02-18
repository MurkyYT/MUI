#include "..\MUI\MUI.h"
#include "resource.h"

using namespace MUI;
void CheckBoxClick();
void ButtonClick();
void RadioGroupChanged();
void WindowClose();
void OpenGridExample();
void InitializeGridDemoWindow();
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
HINSTANCE m_hInstance;
Grid grid;
Window* gridWindow;
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    m_hInstance = hInstance;

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
    chkbox.SetChecked(1);
    grid.AddColumn(0, L"Auto");
    grid.AddColumn(0, L"*");
    grid.AddColumn(0, L"Auto");
    lstView.SetHorizontalAligment(Stretch);
    lstView.SetVerticalAligment(Stretch);
    block.SetVerticalAligment(Center);
    chkbox.SetVerticalAligment(Center);
    grid.AddItem(&lstView, 0, 1);
    grid.AddItem(&chkbox, 0, 2);
    grid.AddItem(&block, 0, 0);

   /* window.SetBackroundColor(Color::PURPLE);
    window.m_StaticBacgkround = Color::PURPLE;
    window.m_StaticTextColor = Color::LIGHT_PURPLE;
    window.m_EditBacgkround = Color::LIGHT_PURPLE;
    window.m_ButtonBacgkround = Color::WHITE;
    window.m_ButtonTextColor = Color::DARK_PURPLE;*/

    Window mainWindow(hInstance);
    mainWindow.Create(L"MUI Demo", (DWORD)IDI_ICON1);
    mainWindow.Show();
    Button btn(L"Open grid demo", FALSE, 0, 0, 0, 0);
    btn.SetHorizontalAligment(Stretch);
    btn.SetVerticalAligment(Stretch);
    mainWindow.AddComponent(&btn);
    btn.SubscribeToOnClick(OpenGridExample);

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
void InitializeGridDemoWindow()
{
    gridWindow = new Window(m_hInstance);
    gridWindow->SubscribeToOnClose(WindowClose);
    if (!gridWindow->Create(L"MUI Grid Demo", (DWORD)IDI_ICON1)) {
        return;
    }
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
   
    gridWindow->SetGrid(&grid);

    lstView.Clear();
    HICON ico = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    BOOL suc = lstView.AddIcon(ico);
    lstView.AddColumn(L"Test", 100);
    lstView.AddColumn(L"Test2", 100);
    ListItem itm(0, std::vector<std::wstring> {L"TEST1", L"TEST2"});
    lstView.AddItem(&itm);
    ListItem itm2(0, std::vector<std::wstring> {L"TEST3", L"TEST4"});
    lstView.AddItem(&itm2);
}
void OpenGridExample()
{
    if (!gridWindow || gridWindow->IsHidden()) 
    {
        InitializeGridDemoWindow();
        gridWindow->Show();
    }
    else
        gridWindow->Activate();
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