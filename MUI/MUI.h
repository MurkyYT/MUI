#pragma once
#pragma region Includes
#include <Windows.h>
#include <string>
#include <unordered_map>
#pragma endregion
#pragma region Defines
#define WINDOW_CLASS L"MUI_Window"
#define DEFAULT_SIZE 500
#pragma endregion
#pragma region CONFIG
#define NEW_STYLE 1
#pragma endregion
#pragma region INTERNAL
#if NEW_STYLE
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif // NEW_STYLE
#pragma endregion

namespace MUI {
	/*
	Every colorref in MUI
	*/
	class Color {
	public:
		static const COLORREF PURPLE = RGB(43, 45, 92);
		static const COLORREF DARK_PURPLE = RGB(35, 35, 79);
		static const COLORREF LIGHT_PURPLE = RGB(93, 107, 238);
		static const COLORREF WHITE = RGB(255, 255, 255);
		static const COLORREF GREY = RGB(225, 225, 225);
		static const COLORREF GRAY = Color::GREY;
	};
	/*
	Enum for each UIComponent Type
	*/
	enum UIType
	{
		Null,
		UIButton,
		UITextBox,
		UICheckBox,
		UITextBlock,
		UIRadioGroup,
		UIRadioButton
	};
	/*
	Base class for each UIComponent
	*/
	class UIComponent
	{
		friend class Window;
	public:
		BOOL SetStyle(DWORD newStyle)
		{
			style = newStyle;
			BOOL res = SetWindowLong(handle, GWL_STYLE, style) != NULL;
			SendMessage(handle, BM_SETSTYLE, newStyle, TRUE);
			SetWindowPos(handle, HWND_TOPMOST, x, y, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			UpdateWindow(handle);
			return res;
		}
		DWORD GetStyle() {
			LONG style = GetWindowLong(handle, GWL_STYLE);
			return style ? style : this->style;
		}
	protected:
		UIType type = Null;
		HWND handle = NULL;
		HWND windowHandle = NULL;
		DWORD id = NULL;
		LPCWSTR m_ClassName = NULL;
		LPCWSTR m_WindowName = NULL;
		DWORD style = NULL;
		COLORREF backgroundColor = NULL;
		UIComponent* parent = NULL;
		int x, y, width, height;
		void* onClick = NULL;
	};
	/*
	Class declarations of each UIComponent
	*/

	class RadioButton : public MUI::UIComponent
	{
		friend class RadioGroup;
	public:
		RadioButton(LPCWSTR text, int x, int y, int width, int height);
		void SubscribeToOnClick(void* func);
	};
	class RadioGroup : public MUI::UIComponent
	{
	public:
		RadioGroup(LPCWSTR text, int x, int y, int width, int height);
		int CurrentRadioButton();
		void SubscribeToOnChange(void* func);
		void AddRadioButton(RadioButton* button);
	private:
		std::vector<RadioButton*> m_Buttons;
	};
	class Button : public MUI::UIComponent
	{
	public:
		Button(LPCWSTR text, BOOL customColors, int x, int y, int width, int height);
		void SubscribeToOnClick(void* func);
	};
	class CheckBox : public MUI::UIComponent
	{
	public:
		CheckBox(LPCWSTR text, int x, int y, int width, int height);
		void SubscribeToOnClick(void* func);
		BOOL IsChecked();
		void SetChecked(BOOL checked);
	};
	class TextBox : public MUI::UIComponent
	{
	public:
		TextBox(LPCWSTR text, BOOL number, int x, int y, int width, int height);
		std::wstring GetText();
		int GetNumber();
	private:
		BOOL isNum;
	};
	class TextBlock : public MUI::UIComponent
	{
	public:
		TextBlock(LPCWSTR text, int x, int y, int width, int height);
		BOOL SetText(LPCWSTR text);
		LPCWSTR GetText();
	};
	/*
	Base MUI window class
	*/
	class Window
	{
	public:
		Window(HINSTANCE hInstance);
		~Window();

		POINT MinSize
		{
			MinSize.x = GetSystemMetrics(SM_CXMINTRACK),
			MinSize.y = GetSystemMetrics(SM_CYMINTRACK)
		};
		POINT MaxSize{
			MaxSize.x = GetSystemMetrics(SM_CXMAXTRACK),
			MaxSize.y = GetSystemMetrics(SM_CYMAXTRACK)
		};
		BOOL Create(const wchar_t* title, int width, int height, DWORD iconId);
		BOOL Create(const wchar_t* title, DWORD iconId);
		BOOL AddComponent(UIComponent* comp);
		void AddComponents(std::vector<UIComponent*> comps);
		void Show(int cmdShow);
		COLORREF m_StaticBacgkround = NULL;
		COLORREF m_StaticTextColor = NULL;

		COLORREF m_EditBacgkround = NULL;
		COLORREF m_EditTextColor = NULL;

		COLORREF m_ButtonBacgkround = NULL;
		COLORREF m_ButtonTextColor = NULL;
		void SetBackroundColor(COLORREF color);
		BOOL Activate();
		void Show();
		void Hide();
	private:
		std::unordered_map<uint64_t, UIComponent*> m_Assets;
		UINT m_Index = 0;
		std::vector<uint64_t> m_UnusedIndexes;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		HWND m_hWnd = NULL;
		HINSTANCE m_hInstance = NULL;
		HFONT m_hFont = NULL;
		HBRUSH m_hBrushBackground = NULL;
		static Window* w_GetObject(HWND hwnd);
		void v_RegisterClass(const wchar_t* name, DWORD iconId);
		void OnCommand(WPARAM wParam, LPARAM lParam);
		void OnDestroy();
		LRESULT OnColorStatic(WPARAM wParam);
		LRESULT OnColorEdit(WPARAM wParam);
		LRESULT OnColorButton(WPARAM wParam);
		LRESULT OnDraw(WPARAM wParam, LPARAM lParam);
		void OnCreate();
	};
}