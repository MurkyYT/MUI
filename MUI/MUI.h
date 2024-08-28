#pragma once
#include <vector>
#pragma region Includes
#include <Windows.h>
#include <string>
#include <unordered_map>
#include <commctrl.h>
#include <dwmapi.h>
#include <functional>
#include <memory>
#pragma comment (lib, "dwmapi")
#pragma comment (lib, "comctl32")
#pragma endregion
#pragma region Defines
#define WINDOW_CLASS L"MUI_Window"
#define DEFAULT_SIZE 500
#define DIVIDER_CLASS_NAME L"MUI_Divider"
#define DIVIDER_NOTIFY_MSG (WM_APP)
#pragma endregion
#pragma region CONFIG
#define NEW_STYLE 1
#define EXIT_ON_CLOSE 1
#define DIVIDER_SIZE 4
#define USE_MULTIPLE_WINDOW_CLASSES 0
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
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#pragma endregion
namespace MUI {
	static inline bool IsDarkMode()
	{
		DWORD vType;
		DWORD vLen = 0;
		DWORD val = 0;
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		if ((RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, &vType, NULL, &vLen) == ERROR_SUCCESS) && (vType == REG_DWORD)) {
			RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, &vType, (LPBYTE)&val, &vLen);
			RegCloseKey(hKey);
			return val == 0;
		}
		return FALSE;
	};
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
		UIImage,
		UIMenu,
		UISeparator,
		UIMenuBar,
		UIComboBox
	};
	/*
	Base class for each UIComponent
	*/

	class UIComponent
	{
		friend class Window;
		friend class Grid;
		friend class GridRow;
		friend class GridColumn;
		friend class ContextMenu;
	public:
		virtual ~UIComponent() {};
		BOOL SetStyle(DWORD newStyle);
		DWORD GetStyle();
		void SetColumnSpan(int span) { this->columnSpan = max(0, span); }
		void SetRowSpan(int span) { this->rowSpan = max(0, span); }
		void SetVerticalAligment(Aligment alg);
		void SetHorizontalAligment(Aligment alg);
		void SetMargin(Margin m);
		void Hide();
		void Show();
		BOOL IsHidden();
	private:
		int GetFullWidth() { return this->width + this->m_margin.left - this->m_margin.right; }
		int GetFullHeight() { return this->height + this->m_margin.top - this->m_margin.bottom; }
	protected:
		static
			void S_HandleEvents(UIComponent* o, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			o->HandleEvents(uMsg,wParam,lParam);
		}
		virtual void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
		virtual void reposition(int h, int w,int x,int y);
		void UpdateHorizontalAligment(POINT& pos, int w,int x);
		void UpdateVerticalAligment(POINT& pos, int h,int y);
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
		int x, y, width, height = 0;
		int columnSpan = 0;
		int rowSpan = 0;
	};
	typedef struct
	{
		UINT    msg;
		WPARAM  wParam;
		LPARAM  lParam;
	} EventArgs_t;
	typedef std::function<void(UIComponent* const sender, EventArgs_t info)> EventCallback_t;
	class Separator : public MUI::UIComponent
	{
	public:
		Separator();
	};
	enum ComboBoxStyle {
		Simple = CBS_SIMPLE,
		DropDown = CBS_DROPDOWN,
		DropDownList = CBS_DROPDOWNLIST
	};
	/*
	* https://learn.microsoft.com/en-us/windows/win32/controls/create-a-simple-combo-box
	* https://learn.microsoft.com/en-us/windows/win32/controls/about-combo-boxes
	*/
	class ComboBox : public MUI::UIComponent
	{
	public:
		ComboBox(std::vector<std::wstring> items, ComboBoxStyle style);
		ComboBox(ComboBoxStyle style);
		std::wstring GetSelectedString();
		int GetSelectedIndex();
		std::vector<std::wstring> GetItems();
		void AddItem(std::wstring item);
		void DeleteItem(std::wstring item);
		void DeleteItem(int i);
	private:
		std::vector<std::wstring> m_Items;
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
	class Menu : public MUI::UIComponent
	{
		friend class Window;
		friend class ContextMenu;
		friend class MenuBar;
	public:
		~Menu();
		Menu(LPCWSTR text);
		void Add(Menu* menu);
		void Add(Separator* sep);
		BOOL IsChecked();
		void SetChecked(BOOL checked);
		void SetEnabled(BOOL enabled);
		EventCallback_t OnClick;
	private:
		BOOL used = FALSE;
		HMENU m_hMenu;
		HMENU m_ParentHMENU;
		UINT m_Index = 1;
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		std::vector<Menu*> m_menus;
		std::vector<Separator*> m_separators;
		std::vector<UIComponent*> m_childs;
	};
	class ContextMenu : private MUI::UIComponent
	{
		friend class Window;
	public:
		~ContextMenu();
		ContextMenu();
		void Add(Menu* menu);
		void Add(Separator* sep);
		void Open();
	private:
		std::vector<Menu*> m_menus;
		std::vector<UIComponent*> m_childs;
		std::unordered_map<uint64_t, UIComponent*> m_Assets;
		static LRESULT CALLBACK SubclassWndProc(HWND hwnd, UINT wm, WPARAM wParam, LPARAM lParam);
		void AppendChilds(Menu* menu, HMENU hMenu);
		UINT m_Index = 1;
		HMENU m_hPopUp;
	};
	class MenuBar /*: public MUI::UIComponent*/
	{
		friend class Window;
	public:
		~MenuBar();
		void Add(Menu* menu);
	private:
		std::vector<Menu*> m_menus;
	};
	/*Every grid related thingy*/
	struct GridItem
	{
		friend class Grid;
	public:
		UIComponent* GetComponent() { return component; }
	private:
		int row;
		int column;
		UIComponent* component;
	};
	class GridRow {
		friend class Window;
		friend class Grid;
	public:
		int GetY() { return this->y; }
	private:
		int y;
		double perc;
		int height;
		const wchar_t* text_height;
		std::vector<UIComponent*> components;
		int GetBiggestHeight()
		{
			int max = 0;
			for (UIComponent* comp : this->components)
				if (max < comp->GetFullHeight())
					max = comp->GetFullHeight();
			return max;
		}
	};
	class GridColumn {
		friend class Window;
		friend class Grid;
	public:
		int GetX() { return this->x; }
	private:
		int x;
		double perc;
		int width;
		const wchar_t* text_width;
		std::vector<UIComponent*> components;
		int GetBiggestWidth()
		{
			int max = 0;
			for (UIComponent* comp : this->components)
				if (max < comp->GetFullWidth())
					max = comp->GetFullWidth();
			return max;
		}
	};
	class Divider {
	    friend class Window;
	    friend class Grid;
	private:
	    HWND hwnd;
		LPARAM strct = NULL;
		POINT start;
        int width;
        int height;
		int row,column;
	    int x;
	    int y;
		BOOL isVertical;
	};
	class Grid {
		friend class Window;
	public:
		/*~Grid();*/
		Grid();
		std::vector<UIComponent*> GetComponents();
		std::vector<std::shared_ptr<GridItem>> GetItems() { return this->m_items; }
		std::vector<std::shared_ptr<Divider>> GetDividers();
		void AddColumn(int x,const wchar_t* width);
		void AddDivider(int column,int row, BOOL isVertical = FALSE);
		void AddRow(int y,const wchar_t* height);
		void AddItem(UIComponent* comp, int row, int column);
	private:
		void CalculateStarPercentages();
		void Reorder(HWND windowHandle,BOOL firstRun = FALSE);
		void Reposition(GridItem* itm);
		std::vector<std::shared_ptr<GridRow>> m_rows;
		std::vector<std::shared_ptr<GridColumn>> m_columns;
		std::vector<std::shared_ptr<GridItem>> m_items;
		std::unordered_map<int, std::unordered_map<int,std::shared_ptr<Divider>>> rowColToDivider;
		BOOL m_AddedCustomRow = FALSE;
		BOOL m_AddedCustomColumn = FALSE;
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
		~ListItem();
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
		~ListView();
		ListView(int x, int y, int width, int height, BOOL autoWidth = TRUE);
		BOOL AddColumn(const wchar_t* title, int length = 100);
		BOOL AddItem(ListItem* item);
		BOOL AddIcon(HICON icon);
		ListItem* GetSelected();
		ListItem* GetItemAt(int i);
		int GetSelectedIndex();
		int FreeItemIndex() { return this->itemIndex; }
		void DeleteIconAt(int i);
		void ClearIcons();
		void ClearColumns();
		void DeleteItemAt(int i);
		void ClearItems();
		void Clear();
		EventCallback_t RightClick{ NULL };
	private:
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		std::vector<ListItem*> m_Items;
		UINT columnIndex;
		BOOL autoWidth;
		UINT itemIndex;
		HIMAGELIST hLarge;   // Image list for icon view.
		HIMAGELIST hSmall;   // Image list for other views.
	};
	class RadioButton : public MUI::UIComponent
	{
		friend class RadioGroup;
	public:
		RadioButton(LPCWSTR text, int x, int y, int width, int height);
		EventCallback_t OnClick;
	private:
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
	class RadioGroup : public MUI::UIComponent
	{
	public:
		RadioGroup(LPCWSTR text, int x, int y, int width, int height);
		int CurrentRadioButton();
		void AddRadioButton(RadioButton* button);
		EventCallback_t OnChange{ NULL };
	private:
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		std::vector<RadioButton*> m_Buttons;
	};
	class Button : public MUI::UIComponent
	{
	public:
		Button(LPCWSTR text, BOOL customColors, int x, int y, int width, int height);
		EventCallback_t OnClick{ NULL };
	private:
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
	class CheckBox : public MUI::UIComponent
	{
	public:
		CheckBox(LPCWSTR text, int x, int y, int width, int height);
		BOOL IsChecked();
		void SetChecked(BOOL checked);
		EventCallback_t OnClick{ NULL };
	private:
		void HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
	class TextBox : public MUI::UIComponent
	{
	public:
		TextBox(LPCWSTR text, BOOL number, int x, int y, int width, int height);
		std::wstring GetText();
		int GetNumber();
		/*void EnableHorizontalScrollbar(BOOL isEnabled);
		void EnableVerticalScrollbar(BOOL isEnabled);
		BOOL HorizontalScrollbarEnabled();
		BOOL VerticalScrollbarEnabled();*/
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
		Window();
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
		void SetGrid(Grid* grid);
		void SetMenuBar(MenuBar* menu);
		void ToggleGrid() { this->b_useGrid = !this->b_useGrid; }
		void AddComponents(std::vector<UIComponent*> comps);
		COLORREF m_StaticBacgkround = NULL;
		COLORREF m_StaticTextColor = NULL;

		COLORREF m_EditBacgkround = NULL;
		COLORREF m_EditTextColor = NULL;

		COLORREF m_ButtonBacgkround = NULL;
		COLORREF m_ButtonTextColor = NULL;

		HWND GetHWND() { return m_hWnd; }
		HINSTANCE GetHINSTACE() { return m_hInstance; }
		void Close() { DestroyWindow(m_hWnd); }
		EventCallback_t OnClose{ NULL };
		SIZE GetSize()
		{
			RECT rect;
			if (GetWindowRect(m_hWnd, &rect))
			{
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				SIZE res{};
				res.cx = width;
				res.cy = height;
				return res;
			}
			return SIZE();
		}
		void SetBackroundColor(COLORREF color);
		BOOL Activate();
		void HideAll();
		void ShowAll();
		void Show();
		void Hide();
		BOOL IsHidden() { return !IsWindowVisible(m_hWnd); }
	private:
#ifdef DEBUG
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;
#endif // DEBUG
		void AppendChilds(Menu* menu, HMENU hMenu);
		void RepositionComponents();
		BOOL b_useGrid = FALSE;
		const wchar_t* m_title;
		int m_width, m_height;
		DWORD m_iconId;
		std::shared_ptr<Grid> m_grid = NULL;
		std::shared_ptr<MenuBar> m_dockMenu = NULL;
		std::unordered_map<uint64_t, std::shared_ptr<UIComponent>> m_Assets;
		UINT m_Index = 1;
		std::vector<uint64_t> m_UnusedIndexes;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		HWND m_hWnd = NULL;
		HINSTANCE m_hInstance = NULL;
		HFONT m_hFont = NULL;
		HBRUSH m_hBrushBackground = NULL;
		COLORREF m_cBackground = MUI::Color::WHITE;
		static Window* w_GetObject(HWND hwnd);
		void v_RegisterClass(const wchar_t* name, DWORD iconId);
		void OnCommand(UINT uMsg,WPARAM wParam, LPARAM lParam);
		void OnDestroy();
		LRESULT OnColorStatic(WPARAM wParam);
		LRESULT OnColorEdit(WPARAM wParam);
		LRESULT OnColorButton(WPARAM wParam);
		LRESULT OnDraw(WPARAM wParam, LPARAM lParam);
		void OnCreate();
		BOOL m_Destroyed = FALSE;
		static int m_Windows;
	};
}
