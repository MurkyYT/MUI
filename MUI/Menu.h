#pragma once

#include "./UIElement.h"
#include "./Events.h"

#include <memory>
#include <vector>

namespace mui
{
    class MenuItem;
    class MenuSeparator;
    class Menu;
    class ContextMenu;

    enum class MenuItemType
    {
        Normal,
        Separator,
        Submenu
    };

    class MenuSeparator
    {
    public:
        MenuSeparator() = default;
        ~MenuSeparator() = default;

        MenuItemType GetType() const { return MenuItemType::Separator; }
    };

    class MenuItem
    {
        friend class Menu;
        friend class ContextMenu;
    public:
        MenuItem(const std::wstring& text = L"");
        ~MenuItem();

        void SetText(const std::wstring& text);
        const std::wstring& GetText() const { return m_text; }

        void SetEnabled(bool enabled);
        bool IsEnabled() const { return m_enabled; }

        void SetChecked(bool checked);
        bool IsChecked() const { return m_checked; }

        void SetShortcut(const std::wstring& shortcut) { m_shortcut = shortcut; }
        const std::wstring& GetShortcut() const { return m_shortcut; }

        void AddMenuItem(std::shared_ptr<MenuItem> item);
        void AddSeparator();
        void RemoveMenuItem(std::shared_ptr<MenuItem> item);
        void ClearMenuItems();

        const std::vector<std::shared_ptr<MenuItem>>& GetMenuItems() const { return m_subItems; }
        const std::vector<std::shared_ptr<MenuSeparator>>& GetSeparators() const { return m_separators; }

        bool HasSubMenu() const { return !m_subItems.empty() || !m_separators.empty(); }
        MenuItemType GetType() const { return HasSubMenu() ? MenuItemType::Submenu : MenuItemType::Normal; }

        EventCallback_t OnClick;

        void SetMenuID(UINT id) { m_menuID = id; }
        UINT GetMenuID() const { return m_menuID; }

        void SetParentMenu(Menu* parent) { m_parentMenu = parent; }
        void SetParentContextMenu(ContextMenu* parent) { m_parentContextMenu = parent; }
        Menu* GetParentMenu() const { return m_parentMenu; }
        ContextMenu* GetParentContextMenu() const { return m_parentContextMenu; }

        HMENU GetNativeHandle() const { return m_hSubmenu; }
        void SetNativeHandle(HMENU hMenu) { m_hSubmenu = hMenu; }

    private:
        std::wstring m_text;
        std::wstring m_shortcut;
        bool m_enabled = true;
        bool m_checked = false;

        UINT m_menuID = 0;
        Menu* m_parentMenu = nullptr;
        ContextMenu* m_parentContextMenu = nullptr;
        HMENU m_hSubmenu = nullptr;

        std::vector<std::shared_ptr<MenuItem>> m_subItems;
        std::vector<std::shared_ptr<MenuSeparator>> m_separators;

        struct MenuElement
        {
            enum Type { Item, Separator } type;
            int index;
        };
        std::vector<MenuElement> m_insertionOrder;

        void UpdateNativeMenu();
    };

    class Menu
    {
    public:
        Menu();
        virtual ~Menu();

        void AddMenuItem(std::shared_ptr<MenuItem> item);
        void AddSeparator();
        void RemoveMenuItem(std::shared_ptr<MenuItem> item);
        void ClearMenuItems();

        const std::vector<std::shared_ptr<MenuItem>>& GetMenuItems() const { return m_menuItems; }
        const std::vector<std::shared_ptr<MenuSeparator>>& GetSeparators() const { return m_separators; }

        void AttachToWindow(HWND hWnd);
        void DetachFromWindow(HWND hWnd);

        HMENU GetNativeHandle() const { return m_hMenu; }

        bool HandleMenuCommand(UINT menuID);

        void BuildNativeMenu();

        void SetTextColor(COLORREF color) { m_textColor = color; }
        void SetBackgroundColor(COLORREF color) { m_barBackgroundColor = color; }
        void SetBottomBarColor(COLORREF color) { m_bottomBarColor = color; }
        void SetItemBackgroundColor(COLORREF color) { m_itemBackgroundColor = color; }
        void SetItemBackgroundHotColor(COLORREF color) { m_itemBackgroundHotColor = color; }
        void SetItemBackgroundSelectedColor(COLORREF color) { m_itemBackgroundSelectedColor = color; }

        COLORREF GetTextColor() { return m_textColor; }
        COLORREF GetBackgroundColor() { return m_barBackgroundColor; }
        COLORREF GetBottomBarColor() { return m_bottomBarColor; }
        COLORREF GetItemBackgroundColor() { return m_itemBackgroundColor; }
        COLORREF GetItemBackgroundHotColor() { return m_itemBackgroundHotColor; }
        COLORREF GetItemBackgroundSelectedColor() { return m_itemBackgroundSelectedColor; }

    private:
        HMENU m_hMenu = nullptr;
        HWND m_attachedWindow = nullptr;

        std::vector<std::shared_ptr<MenuItem>> m_menuItems;
        std::vector<std::shared_ptr<MenuSeparator>> m_separators;

        COLORREF m_textColor = RGB(0, 0, 0);
        COLORREF m_barBackgroundColor = RGB(255,255,255);
        COLORREF m_bottomBarColor = RGB(240,240,240);
        COLORREF m_itemBackgroundColor = RGB(255, 255, 255);
        COLORREF m_itemBackgroundHotColor = RGB(245, 245, 245);
        COLORREF m_itemBackgroundSelectedColor = RGB(249, 249, 249);

        struct MenuElement
        {
            enum Type { Item, Separator } type;
            int index;
        };
        std::vector<MenuElement> m_insertionOrder;

        static UINT s_nextMenuID;
        UINT GetNextMenuID() { return ++s_nextMenuID; }

        void BuildMenuItemRecursive(HMENU hMenu, MenuItem* item);
        void BuildSeparator(HMENU hMenu);
        MenuItem* FindMenuItemByID(UINT id);
        void AssignMenuIDs();
        void UpdateMenuItemStates();
    };

    class ContextMenu
    {
        friend class MenuItem;
    public:
        ContextMenu();
        ~ContextMenu();

        void AddMenuItem(std::shared_ptr<MenuItem> item);
        void AddSeparator();
        void RemoveMenuItem(std::shared_ptr<MenuItem> item);
        void ClearMenuItems();

        const std::vector<std::shared_ptr<MenuItem>>& GetMenuItems() const { return m_menuItems; }
        const std::vector<std::shared_ptr<MenuSeparator>>& GetSeparators() const { return m_separators; }

        void Open();
        void Open(int x, int y);
        void Close();

        bool IsOpen() const { return m_isOpen; }

        bool HandleMenuCommand(UINT menuID);

    private:
        HWND m_dummyWindow = nullptr;
        HMENU m_hPopupMenu = nullptr;
        bool m_isOpen = false;

        std::vector<std::shared_ptr<MenuItem>> m_menuItems;
        std::vector<std::shared_ptr<MenuSeparator>> m_separators;

        struct MenuElement
        {
            enum Type { Item, Separator } type;
            int index;
        };
        std::vector<MenuElement> m_insertionOrder;

        static UINT s_nextMenuID;
        UINT GetNextMenuID() { return ++s_nextMenuID; }

        void CreateDummyWindow();
        void DestroyDummyWindow();
        void BuildNativeMenu();
        void BuildMenuItemRecursive(HMENU hMenu, MenuItem* item);
        void BuildSeparator(HMENU hMenu);
        MenuItem* FindMenuItemByID(UINT id);
        void AssignMenuIDs();
        void UpdateMenuItemStates();

        static LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static const wchar_t* GetDummyWindowClass();
        static void RegisterDummyWindowClass();
        static bool s_windowClassRegistered;
    };
}