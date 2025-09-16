#include "Menu.h"
#include <algorithm>

namespace mui
{
    UINT Menu::s_nextMenuID = 1000;
    UINT ContextMenu::s_nextMenuID = 2000;
    bool ContextMenu::s_windowClassRegistered = false;

    MenuItem::MenuItem(const std::wstring& text) : m_text(text)
    {
    }

    MenuItem::~MenuItem()
    {
        if (m_hSubmenu)
        {
            DestroyMenu(m_hSubmenu);
            m_hSubmenu = nullptr;
        }
    }

    void MenuItem::SetText(const std::wstring& text)
    {
        m_text = text;
        UpdateNativeMenu();
    }

    void MenuItem::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
        UpdateNativeMenu();
    }

    void MenuItem::SetChecked(bool checked)
    {
        m_checked = checked;
        UpdateNativeMenu();
    }

    void MenuItem::AddMenuItem(std::shared_ptr<MenuItem> item)
    {
        if (item)
        {
            m_subItems.push_back(item);
            m_insertionOrder.push_back({ MenuElement::Item, (int)(m_subItems.size() - 1) });

            if (m_parentMenu)
            {
                item->SetParentMenu(m_parentMenu);
            }
            else if (m_parentContextMenu)
            {
                item->SetParentContextMenu(m_parentContextMenu);
            }

            UpdateNativeMenu();
        }
    }

    void MenuItem::AddSeparator()
    {
        auto separator = std::make_shared<MenuSeparator>();
        m_separators.push_back(separator);
        m_insertionOrder.push_back({ MenuElement::Separator, (int)(m_separators.size() - 1) });
        UpdateNativeMenu();
    }

    void MenuItem::RemoveMenuItem(std::shared_ptr<MenuItem> item)
    {
        auto it = std::find(m_subItems.begin(), m_subItems.end(), item);
        if (it != m_subItems.end())
        {
            int index = (int)(it - m_subItems.begin());
            m_subItems.erase(it);

            for (auto orderIt = m_insertionOrder.begin(); orderIt != m_insertionOrder.end(); ++orderIt)
            {
                if (orderIt->type == MenuElement::Item && orderIt->index == index)
                {
                    m_insertionOrder.erase(orderIt);
                    break;
                }
            }

            for (auto& element : m_insertionOrder)
            {
                if (element.type == MenuElement::Item && element.index > index)
                {
                    element.index--;
                }
            }

            UpdateNativeMenu();
        }
    }

    void MenuItem::ClearMenuItems()
    {
        m_subItems.clear();
        m_separators.clear();
        m_insertionOrder.clear();
        UpdateNativeMenu();
    }

    void MenuItem::UpdateNativeMenu()
    {
        if (m_parentMenu)
        {
            m_parentMenu->BuildNativeMenu();
        }
        else if (m_parentContextMenu)
        {
            m_parentContextMenu->BuildNativeMenu();
        }
    }

    Menu::Menu()
    {
        m_hMenu = CreateMenu();
    }

    Menu::~Menu()
    {
        if (m_attachedWindow && m_hMenu)
        {
            SetMenu(m_attachedWindow, nullptr);
        }

        if (m_hMenu)
        {
            DestroyMenu(m_hMenu);
            m_hMenu = nullptr;
        }
    }

    void Menu::AddMenuItem(std::shared_ptr<MenuItem> item)
    {
        if (item)
        {
            m_menuItems.push_back(item);
            m_insertionOrder.push_back({ MenuElement::Item, (int)(m_menuItems.size() - 1) });
            item->SetParentMenu(this);
            BuildNativeMenu();
        }
    }

    void Menu::AddSeparator()
    {
        auto separator = std::make_shared<MenuSeparator>();
        m_separators.push_back(separator);
        m_insertionOrder.push_back({ MenuElement::Separator, (int)(m_separators.size() - 1) });
        BuildNativeMenu();
    }

    void Menu::RemoveMenuItem(std::shared_ptr<MenuItem> item)
    {
        auto it = std::find(m_menuItems.begin(), m_menuItems.end(), item);
        if (it != m_menuItems.end())
        {
            int index = (int)(it - m_menuItems.begin());
            m_menuItems.erase(it);

            for (auto orderIt = m_insertionOrder.begin(); orderIt != m_insertionOrder.end(); ++orderIt)
            {
                if (orderIt->type == MenuElement::Item && orderIt->index == index)
                {
                    m_insertionOrder.erase(orderIt);
                    break;
                }
            }

            for (auto& element : m_insertionOrder)
            {
                if (element.type == MenuElement::Item && element.index > index)
                {
                    element.index--;
                }
            }

            BuildNativeMenu();
        }
    }

    void Menu::ClearMenuItems()
    {
        m_menuItems.clear();
        m_separators.clear();
        m_insertionOrder.clear();
        BuildNativeMenu();
    }

    void Menu::AttachToWindow(HWND hWnd)
    {
        if (m_hMenu && hWnd)
        {
            if (m_attachedWindow)
            {
                DetachFromWindow(m_attachedWindow);
            }

            SetMenu(hWnd, m_hMenu);
            DrawMenuBar(hWnd);
            m_attachedWindow = hWnd;
        }
    }

    void Menu::DetachFromWindow(HWND hWnd)
    {
        if (m_attachedWindow == hWnd)
        {
            SetMenu(hWnd, nullptr);
            DrawMenuBar(hWnd);
            m_attachedWindow = nullptr;
        }
    }

    bool Menu::HandleMenuCommand(UINT menuID)
    {
        MenuItem* item = FindMenuItemByID(menuID);

        if (item && item->OnClick)
        {
            item->OnClick(item, NULL);
            return true;
        }

        return false;
    }

    void Menu::BuildNativeMenu()
    {
        if (!m_hMenu) return;

        while (GetMenuItemCount(m_hMenu) > 0)
        {
            RemoveMenu(m_hMenu, 0, MF_BYPOSITION);
        }

        AssignMenuIDs();

        for (const auto& element : m_insertionOrder)
        {
            if (element.type == MenuElement::Item)
            {
                BuildMenuItemRecursive(m_hMenu, m_menuItems[element.index].get());
            }
            else
            {
                BuildSeparator(m_hMenu);
            }
        }

        if (m_attachedWindow)
        {
            DrawMenuBar(m_attachedWindow);
        }
    }

    void Menu::BuildMenuItemRecursive(HMENU hMenu, MenuItem* item)
    {
        if (item->HasSubMenu())
        {
            if (item->GetNativeHandle())
            {
                DestroyMenu(item->GetNativeHandle());
            }

            HMENU hSubmenu = CreatePopupMenu();
            item->SetNativeHandle(hSubmenu);

            for (const auto& element : item->m_insertionOrder)
            {
                if (element.type == MenuElement::Item)
                {
                    BuildMenuItemRecursive(hSubmenu, item->GetMenuItems()[element.index].get());
                }
                else
                {
                    BuildSeparator(hSubmenu);
                }
            }

            UINT flags = MF_POPUP;
            if (!item->IsEnabled()) flags |= MF_GRAYED;

            AppendMenuW(hMenu, flags, (UINT_PTR)hSubmenu, item->GetText().c_str());
        }
        else
        {
            UINT flags = MF_STRING;
            if (!item->IsEnabled()) flags |= MF_GRAYED;
            if (item->IsChecked()) flags |= MF_CHECKED;

            std::wstring displayText = item->GetText();
            if (!item->GetShortcut().empty())
            {
                displayText += L"\t" + item->GetShortcut();
            }

            AppendMenuW(hMenu, flags, item->GetMenuID(), displayText.c_str());
        }
    }

    void Menu::BuildSeparator(HMENU hMenu)
    {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    }

    MenuItem* Menu::FindMenuItemByID(UINT id)
    {
        std::function<MenuItem* (const std::vector<std::shared_ptr<MenuItem>>&)> searchItems =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items) -> MenuItem*
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() == id)
                        return item.get();

                    if (item->HasSubMenu())
                    {
                        MenuItem* found = searchItems(item->GetMenuItems());
                        if (found) return found;
                    }
                }
                return nullptr;
            };

        return searchItems(m_menuItems);
    }

    void Menu::AssignMenuIDs()
    {
        std::function<void(const std::vector<std::shared_ptr<MenuItem>>&)> assignIDs =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items)
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() == 0)
                    {
                        item->SetMenuID(GetNextMenuID());
                    }

                    if (item->HasSubMenu())
                    {
                        assignIDs(item->GetMenuItems());
                    }
                }
            };

        assignIDs(m_menuItems);
    }

    void Menu::UpdateMenuItemStates()
    {
        if (!m_hMenu) return;

        std::function<void(const std::vector<std::shared_ptr<MenuItem>>&)> updateStates =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items)
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() > 0)
                    {
                        EnableMenuItem(m_hMenu, item->GetMenuID(),
                            item->IsEnabled() ? MF_ENABLED : MF_GRAYED);
                        CheckMenuItem(m_hMenu, item->GetMenuID(),
                            item->IsChecked() ? MF_CHECKED : MF_UNCHECKED);
                    }

                    if (item->HasSubMenu())
                    {
                        updateStates(item->GetMenuItems());
                    }
                }
            };

        updateStates(m_menuItems);
    }

    ContextMenu::ContextMenu()
    {
        RegisterDummyWindowClass();
        CreateDummyWindow();
        m_hPopupMenu = CreatePopupMenu();
    }

    ContextMenu::~ContextMenu()
    {
        if (m_hPopupMenu)
        {
            DestroyMenu(m_hPopupMenu);
            m_hPopupMenu = nullptr;
        }

        DestroyDummyWindow();
    }

    void ContextMenu::AddMenuItem(std::shared_ptr<MenuItem> item)
    {
        if (item)
        {
            m_menuItems.push_back(item);
            m_insertionOrder.push_back({ MenuElement::Item, (int)(m_menuItems.size() - 1) });
            item->SetParentContextMenu(this);
            BuildNativeMenu();
        }
    }

    void ContextMenu::AddSeparator()
    {
        auto separator = std::make_shared<MenuSeparator>();
        m_separators.push_back(separator);
        m_insertionOrder.push_back({ MenuElement::Separator, (int)(m_separators.size() - 1) });
        BuildNativeMenu();
    }

    void ContextMenu::RemoveMenuItem(std::shared_ptr<MenuItem> item)
    {
        auto it = std::find(m_menuItems.begin(), m_menuItems.end(), item);
        if (it != m_menuItems.end())
        {
            int index = (int)(it - m_menuItems.begin());
            m_menuItems.erase(it);

            for (auto orderIt = m_insertionOrder.begin(); orderIt != m_insertionOrder.end(); ++orderIt)
            {
                if (orderIt->type == MenuElement::Item && orderIt->index == index)
                {
                    m_insertionOrder.erase(orderIt);
                    break;
                }
            }

            for (auto& element : m_insertionOrder)
            {
                if (element.type == MenuElement::Item && element.index > index)
                {
                    element.index--;
                }
            }

            BuildNativeMenu();
        }
    }

    void ContextMenu::ClearMenuItems()
    {
        m_menuItems.clear();
        m_separators.clear();
        m_insertionOrder.clear();
        BuildNativeMenu();
    }

    void ContextMenu::Open()
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        Open(cursorPos.x, cursorPos.y);
    }

    void ContextMenu::Open(int x, int y)
    {
        if (!m_hPopupMenu || m_isOpen) return;

        UpdateMenuItemStates();
        m_isOpen = true;

        TrackPopupMenuEx(m_hPopupMenu,
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
            x, y, m_dummyWindow, nullptr);

        m_isOpen = false;
    }

    void ContextMenu::Close()
    {
        if (m_isOpen)
        {
            EndMenu();
            m_isOpen = false;
        }
    }

    bool ContextMenu::HandleMenuCommand(UINT menuID)
    {
        MenuItem* item = FindMenuItemByID(menuID);

        if (item && item->OnClick)
        {
            item->OnClick(item, NULL);
            return true;
        }

        return false;
    }

    void ContextMenu::CreateDummyWindow()
    {
        m_dummyWindow = CreateWindowExW(
            0,
            GetDummyWindowClass(),
            L"",
            0,
            0, 0, 0, 0,
            HWND_MESSAGE,
            nullptr,
            GetModuleHandleW(nullptr),
            this
        );
    }

    void ContextMenu::DestroyDummyWindow()
    {
        if (m_dummyWindow)
        {
            DestroyWindow(m_dummyWindow);
            m_dummyWindow = nullptr;
        }
    }

    void ContextMenu::BuildNativeMenu()
    {
        if (!m_hPopupMenu) return;

        while (GetMenuItemCount(m_hPopupMenu) > 0)
        {
            RemoveMenu(m_hPopupMenu, 0, MF_BYPOSITION);
        }

        AssignMenuIDs();

        for (const auto& element : m_insertionOrder)
        {
            if (element.type == MenuElement::Item)
            {
                BuildMenuItemRecursive(m_hPopupMenu, m_menuItems[element.index].get());
            }
            else
            {
                BuildSeparator(m_hPopupMenu);
            }
        }
    }

    void ContextMenu::BuildMenuItemRecursive(HMENU hMenu, MenuItem* item)
    {
        if (item->HasSubMenu())
        {
            if (item->GetNativeHandle())
            {
                DestroyMenu(item->GetNativeHandle());
            }

            HMENU hSubmenu = CreatePopupMenu();
            item->SetNativeHandle(hSubmenu);

            for (const auto& element : item->m_insertionOrder)
            {
                if (element.type == MenuElement::Item)
                {
                    BuildMenuItemRecursive(hSubmenu, item->GetMenuItems()[element.index].get());
                }
                else
                {
                    BuildSeparator(hSubmenu);
                }
            }

            UINT flags = MF_POPUP;
            if (!item->IsEnabled()) flags |= MF_GRAYED;

            AppendMenuW(hMenu, flags, (UINT_PTR)hSubmenu, item->GetText().c_str());
        }
        else
        {
            UINT flags = MF_STRING;
            if (!item->IsEnabled()) flags |= MF_GRAYED;
            if (item->IsChecked()) flags |= MF_CHECKED;

            std::wstring displayText = item->GetText();
            if (!item->GetShortcut().empty())
            {
                displayText += L"\t" + item->GetShortcut();
            }

            AppendMenuW(hMenu, flags, item->GetMenuID(), displayText.c_str());
        }
    }

    void ContextMenu::BuildSeparator(HMENU hMenu)
    {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    }

    MenuItem* ContextMenu::FindMenuItemByID(UINT id)
    {
        std::function<MenuItem* (const std::vector<std::shared_ptr<MenuItem>>&)> searchItems =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items) -> MenuItem*
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() == id)
                        return item.get();

                    if (item->HasSubMenu())
                    {
                        MenuItem* found = searchItems(item->GetMenuItems());
                        if (found) return found;
                    }
                }
                return nullptr;
            };

        return searchItems(m_menuItems);
    }

    void ContextMenu::AssignMenuIDs()
    {
        std::function<void(const std::vector<std::shared_ptr<MenuItem>>&)> assignIDs =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items)
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() == 0)
                    {
                        item->SetMenuID(GetNextMenuID());
                    }

                    if (item->HasSubMenu())
                    {
                        assignIDs(item->GetMenuItems());
                    }
                }
            };

        assignIDs(m_menuItems);
    }

    void ContextMenu::UpdateMenuItemStates()
    {
        if (!m_hPopupMenu) return;

        std::function<void(const std::vector<std::shared_ptr<MenuItem>>&)> updateStates =
            [&](const std::vector<std::shared_ptr<MenuItem>>& items)
            {
                for (auto& item : items)
                {
                    if (item->GetMenuID() > 0)
                    {
                        EnableMenuItem(m_hPopupMenu, item->GetMenuID(),
                            item->IsEnabled() ? MF_ENABLED : MF_GRAYED);
                        CheckMenuItem(m_hPopupMenu, item->GetMenuID(),
                            item->IsChecked() ? MF_CHECKED : MF_UNCHECKED);
                    }

                    if (item->HasSubMenu())
                    {
                        updateStates(item->GetMenuItems());
                    }
                }
            };

        updateStates(m_menuItems);
    }

    LRESULT CALLBACK ContextMenu::DummyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        ContextMenu* contextMenu = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            contextMenu = reinterpret_cast<ContextMenu*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(contextMenu));
        }
        else
            contextMenu = (ContextMenu*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

        if (contextMenu)
        {
            switch (uMsg)
            {
            case WM_COMMAND:
            {
                UINT menuID = LOWORD(wParam);
                if (contextMenu->HandleMenuCommand(menuID))
                {
                    return 0;
                }
                break;
            }
            case WM_MENUSELECT:
                break;

            default:
                break;
        }
    }

        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    const wchar_t* ContextMenu::GetDummyWindowClass()
    {
        return L"MUI_ContextMenu_DummyWindow";
    }

    void ContextMenu::RegisterDummyWindowClass()
    {
        if (s_windowClassRegistered) return;

        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = DummyWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = GetDummyWindowClass();

        RegisterClassExW(&wc);
        s_windowClassRegistered = true;
    }
}