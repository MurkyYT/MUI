#pragma once
#pragma region Includes
#include <Windows.h>
#include <string>
#include <unordered_map>
#include <commctrl.h> 
#pragma comment (lib, "comctl32")
#pragma endregion
#pragma region Defines
#define WINDOW_CLASS L"MUI_Window"
#define DEFAULT_SIZE 500
#pragma endregion
#pragma region CONFIG
#define NEW_STYLE 1
#define HIDE_ON_CLOSE 0
#pragma endregion
#pragma region INTERNAL
#if NEW_STYLE
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif // NEW_STYLE
#ifndef NDEBUG
#define DEBUG
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#endif
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
	struct Margin
	{
		LONG left, right, top, bottom;
		Margin(
			LONG left = 0, 
			LONG right = 0, 
			LONG top = 0,
			LONG bottom = 0) : left(left), right(right), top(top), bottom(bottom) {}
	};
	/*Enum for each aligment*/
	enum Aligment
	{
		None,
		Top,
		Bottom,
		Left,
		Right,
		Center,
		Stretch
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
		UIRadioButton,
		UIListView,
		UIImage
	};
	/*
	Base class for each UIComponent
	*/
	class UIComponent
	{
		friend class Window;
	public:
		BOOL SetStyle(DWORD newStyle);
		DWORD GetStyle();
		void SetVerticalAligment(Aligment alg);
		void SetHorizontalAligment(Aligment alg);
		void SetMargin(Margin m);
		void Hide();
		void Show();
		BOOL IsHidden();
	protected:
		virtual void reposition(int h, int w);
		void UpdateHorizontalAligment(POINT& pos, int w);
		void UpdateVerticalAligment(POINT& pos, int h);
		UIType type = Null;
		Aligment m_hAligment = None;
		Aligment m_vAligment = None;
		Margin m_margin = {};
		HWND handle = NULL;
		HWND windowHandle = NULL;
		DWORD id = NULL;
		LPCWSTR m_ClassName = NULL;
		LPCWSTR m_WindowName = NULL;
		DWORD style = NULL;
		COLORREF backgroundColor = NULL;
		UIComponent* parent = NULL;
		int x, y, width, height;
		void* onEvent = NULL;
	};
	/*
	Class declarations of each UIComponent
	*/

	class Image : public MUI::UIComponent
	{
	public:
		Image(int x, int y, int width, int height);
		void SetImage(HBITMAP hBmp);
	private:
		HBITMAP m_hBmp;
	};
	class ListItem
	{
		friend class ListView;
	public:
		ListItem(int imageIndex, std::vector<std::wstring> values);
		void SetImageIndex(int index);
		std::vector<std::wstring> GetValues();
		int GetImageIndex();
	private:
		int imageIndex;
		std::vector<std::wstring> values;
	};
	class ListView : public MUI::UIComponent
	{
	public:
		ListView(int x, int y, int width, int height);
		BOOL AddColumn(const wchar_t* title, int length = 100);
		BOOL AddItem(ListItem* item);
		BOOL AddIcon(HICON icon);
		ListItem* GetSelected();
		ListItem* GetItemAt(int i);
		int GetSelectedIndex();
		int FreeItemIndex() { return this->itemIndex; }
		void DeleteIconAt(int i);
		void ClearIcons();
		void DeleteItemAt(int i);
		void ClearItems();
	private:
		std::vector<ListItem> m_Items;
		UINT columnIndex;
		UINT itemIndex;
		HIMAGELIST hLarge;   // Image list for icon view.
		HIMAGELIST hSmall;   // Image list for other views.
	};
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
		void SubscribeToOnClose(void* func) { onClose = func; }
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
		BOOL IsHidden() { return !IsWindowVisible(m_hWnd); }
	private:
#ifdef DEBUG
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;
#endif // DEBUG
		void RepositionComponents();
		void* onClose = NULL;
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