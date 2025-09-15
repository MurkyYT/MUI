#pragma once
#include "IGenerator.h"

class TreeViewGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::shared_ptr<mui::TreeView> treeView = std::make_shared<mui::TreeView>();

        ApplyCommonProperties(treeView);

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                treeView->SetTextColor(color);
            }
        }

        std::wstring expandButtonColor = GetProperty(L"ExpandButtonColor");
        if (!expandButtonColor.empty()) {
            COLORREF color = ParseColor(expandButtonColor);
            if (color != CLR_INVALID) {
                treeView->SetExpandButtonColor(color);
            }
        }

        std::wstring selectedColor = GetProperty(L"SelectedColor");
        if (!selectedColor.empty()) {
            COLORREF color = ParseColor(selectedColor);
            if (color != CLR_INVALID) {
                treeView->SetSelectedColor(color);
            }
        }

        return treeView;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::TreeView> " : L"\t\t") +
            varName + L" = std::make_shared<mui::TreeView>();\r\n";

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

        std::wstring expandButtonColor = GetProperty(L"ExpandButtonColor");
        if (!expandButtonColor.empty()) {
            COLORREF color = ParseColor(expandButtonColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetExpandButtonColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring selectedColor = GetProperty(L"SelectedColor");
        if (!selectedColor.empty()) {
            COLORREF color = ParseColor(selectedColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetSelectedColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        output += GenerateElementSpecificEventHandlers(varName);

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"RightClick", L"DoubleClick", L"OnReturn", L"SelectionChanged" };
    }

    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        const std::vector<std::wstring> events = { L"RightClick", L"DoubleClick", L"OnReturn", L"SelectionChanged" };

        for (const auto& eventName : events) {
            std::wstring handlerName = GetProperty(eventName);
            if (!handlerName.empty()) {
                output += L"\t\t" + varName + L"->" + eventName + L" = std::bind(&" + className + L"::" + handlerName +
                    L", this, std::placeholders::_1, std::placeholders::_2);\r\n";
            }
        }

        return output;
    }
};