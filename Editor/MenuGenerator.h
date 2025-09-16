#pragma once
#include "IGenerator.h"
#include <stdexcept>

class MenuItemGenerator : public UIElementGeneratorBase {
public:
    std::wstring Generate() override {

        if (!m_parent ||
            !(m_parent->GetElementName() == L"Menu" || m_parent->GetElementName() == L"MenuItem")) {
            throw std::runtime_error("<MenuItem> must be child of <Menu> or <MenuItem>");
        }

        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::MenuItem> " : L"\t\t") + varName + L" = std::make_shared<mui::MenuItem>(L\"" + EscapeString(text) + L"\");\r\n";

        std::wstring enabled = GetProperty(L"Enabled");
        if (!enabled.empty()) {
            BOOL isEnabled = (enabled == L"true" || enabled == L"True" || enabled == L"1");
            output += L"\t\t" + varName + L"->SetEnabled(" + (isEnabled ? L"TRUE" : L"FALSE") + L");\r\n";
        }

        std::wstring checked = GetProperty(L"Checked");
        if (!checked.empty()) {
            BOOL isChecked = (checked == L"true" || checked == L"True" || checked == L"1");
            output += L"\t\t" + varName + L"->SetChecked(" + (isChecked ? L"TRUE" : L"FALSE") + L");\r\n";
        }

        std::wstring shortcut = GetProperty(L"Shortcut");
        if (!shortcut.empty()) {
            output += L"\t\t" + varName + L"->SetShortcut(L\"" + EscapeString(shortcut) + L"\");\r\n";
        }

        output += GenerateElementSpecificEventHandlers(varName);

        for (const auto& child : children) {
            output += child->Generate();
            std::wstring childVarName = child->GetVariableName();

            if (child->GetElementName() == L"MenuItem") {
                output += L"\t\t" + varName + L"->AddMenuItem(" + childVarName + L");\r\n";
            }
            else if (child->GetElementName() == L"MenuSeparator") {
                output += L"\t\t" + varName + L"->AddSeparator();\r\n";
            }
        }

        return output;
    }

    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override {
        return NULL;
    }

    std::shared_ptr<mui::MenuItem> CreateMenuItemPreview() {
        std::wstring text = GetProperty(L"Text");
        auto menuItem = std::make_shared<mui::MenuItem>(text);

        std::wstring enabled = GetProperty(L"Enabled");
        if (!enabled.empty()) {
            BOOL isEnabled = (enabled == L"true" || enabled == L"True" || enabled == L"1");
            menuItem->SetEnabled(isEnabled);
        }

        std::wstring checked = GetProperty(L"Checked");
        if (!checked.empty()) {
            BOOL isChecked = (checked == L"true" || checked == L"True" || checked == L"1");
            menuItem->SetChecked(isChecked);
        }

        std::wstring shortcut = GetProperty(L"Shortcut");
        if (!shortcut.empty()) {
            menuItem->SetShortcut(shortcut);
        }

        for (const auto& child : children) {
            if (child->GetElementName() == L"MenuItem") {
                auto childMenuItemGen = std::dynamic_pointer_cast<MenuItemGenerator>(child);
                if (childMenuItemGen) {
                    menuItem->AddMenuItem(childMenuItemGen->CreateMenuItemPreview());
                }
            }
            else if (child->GetElementName() == L"MenuSeparator") {
                menuItem->AddSeparator();
            }
        }

        return menuItem;
    }

protected:
    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        std::wstring onClick = GetProperty(L"OnClick");
        if (!onClick.empty()) {
            output += L"\t\t" + varName + L"->OnClick = std::bind(&" + className + L"::" + onClick +
                L", this, std::placeholders::_1, std::placeholders::_2);\r\n";
        }

        return output;
    }

    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"OnClick" };
    }
};

class MenuGenerator : public UIElementGeneratorBase {
public:
    std::wstring Generate() override {
        if (!m_parent || m_parent->GetElementName() != L"Window") {
            throw std::runtime_error("<Menu> must be direct child of <Window>");
        }

        std::wstring output;
        std::wstring varName = GetVariableName();

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Menu> " : L"\t\t") + varName + L" = std::make_shared<mui::Menu>();\r\n";

        output += GenerateCommonPropertySetters(varName);

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetTextColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring barBackgroundColor = GetProperty(L"BarBackgroundColor");
        if (!barBackgroundColor.empty()) {
            COLORREF color = ParseColor(barBackgroundColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetBarBackgroundColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring bottomBarColor = GetProperty(L"BottomBarColor");
        if (!bottomBarColor.empty()) {
            COLORREF color = ParseColor(bottomBarColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetBottomBarColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring itemBackgroundColor = GetProperty(L"ItemBackgroundColor");
        if (!itemBackgroundColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetItemBackgroundColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring itemBackgroundHotColor = GetProperty(L"ItemBackgroundHotColor");
        if (!itemBackgroundHotColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundHotColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetItemBackgroundHotColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring itemBackgroundSelectedColor = GetProperty(L"ItemBackgroundSelectedColor");
        if (!itemBackgroundSelectedColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundSelectedColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetItemBackgroundSelectedColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        for (const auto& child : children) {
            output += child->Generate();
            std::wstring childVarName = child->GetVariableName();

            if (child->GetElementName() == L"MenuItem") {
                output += L"\t\t" + varName + L"->AddMenuItem(" + childVarName + L");\r\n";
            }
            else if (child->GetElementName() == L"MenuSeparator") {
                output += L"\t\t" + varName + L"->AddSeparator();\r\n";
            }
        }

        return output;
    }

    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override {
        return NULL;
    }

    std::shared_ptr<mui::Menu> CreateMenuPreview() {
        auto menu = std::make_shared<mui::Menu>();

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                menu->SetTextColor(color);
            }
        }

        std::wstring barBackgroundColor = GetProperty(L"BarBackgroundColor");
        if (!barBackgroundColor.empty()) {
            COLORREF color = ParseColor(barBackgroundColor);
            if (color != CLR_INVALID) {
                menu->SetBackgroundColor(color);
            }
        }

        std::wstring bottomBarColor = GetProperty(L"BottomBarColor");
        if (!bottomBarColor.empty()) {
            COLORREF color = ParseColor(bottomBarColor);
            if (color != CLR_INVALID) {
                menu->SetBottomBarColor(color);
            }
        }

        std::wstring itemBackgroundColor = GetProperty(L"ItemBackgroundColor");
        if (!itemBackgroundColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundColor);
            if (color != CLR_INVALID) {
                menu->SetItemBackgroundColor(color);
            }
        }

        std::wstring itemBackgroundHotColor = GetProperty(L"ItemBackgroundHotColor");
        if (!itemBackgroundHotColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundHotColor);
            if (color != CLR_INVALID) {
                menu->SetItemBackgroundHotColor(color);
            }
        }

        std::wstring itemBackgroundSelectedColor = GetProperty(L"ItemBackgroundSelectedColor");
        if (!itemBackgroundSelectedColor.empty()) {
            COLORREF color = ParseColor(itemBackgroundSelectedColor);
            if (color != CLR_INVALID) {
                menu->SetItemBackgroundSelectedColor(color);
            }
        }

        for (const auto& child : children) {
            if (child->GetElementName() == L"MenuItem") {
                auto menuItemGen = std::dynamic_pointer_cast<MenuItemGenerator>(child);
                if (menuItemGen) {
                    menu->AddMenuItem(menuItemGen->CreateMenuItemPreview());
                }
            }
            else if (child->GetElementName() == L"MenuSeparator") {
                menu->AddSeparator();
            }
        }

        return menu;
    }
};

class MenuSeparatorGenerator : public UIElementGeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }

    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override {
        return NULL;
    }
};