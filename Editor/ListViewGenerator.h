#pragma once
#include "IGenerator.h"

class ListViewGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::shared_ptr<mui::ListView> listView = std::make_shared<mui::ListView>();

        ApplyCommonProperties(listView);

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                listView->SetTextColor(color);
            }
        }

        std::wstring separatorColor = GetProperty(L"SeparatorColor");
        if (!separatorColor.empty()) {
            COLORREF color = ParseColor(separatorColor);
            if (color != CLR_INVALID) {
                listView->SetSeparatorColor(color);
            }
        }

        std::wstring hoverColor = GetProperty(L"HoverColor");
        if (!hoverColor.empty()) {
            COLORREF color = ParseColor(hoverColor);
            if (color != CLR_INVALID) {
                listView->SetHoverColor(color);
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"ListView.Column") {
                std::wstring title = child->GetProperty(L"Title");
                if (!title.empty()) {
                    listView->AddColumn(title.c_str());
                }
            }
        }

        std::wstring showColumns = GetProperty(L"ShowColumns");
        if (showColumns == L"false" || showColumns == L"False" || showColumns == L"0") {
            listView->HideColumns();
        }

        return listView;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::ListView> " : L"\t\t") +
            varName + L" = std::make_shared<mui::ListView>();\r\n";

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

        std::wstring separatorColor = GetProperty(L"SeparatorColor");
        if (!separatorColor.empty()) {
            COLORREF color = ParseColor(separatorColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetSeparatorColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring hoverColor = GetProperty(L"HoverColor");
        if (!hoverColor.empty()) {
            COLORREF color = ParseColor(hoverColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetHoverColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"ListView.Column") {
                std::wstring title = child->GetProperty(L"Title");
                std::wstring width = child->GetProperty(L"Width");

                if (!title.empty()) {
                    output += L"\t\t" + varName + L"->AddColumn(L\"" + EscapeString(title) + L"\");\r\n";

                    if (!width.empty()) {
                        output += L"\t\t{\r\n";
                        output += L"\t\t\tint columnIndex = Header_GetItemCount(ListView_GetHeader(" + varName + L"->GetHWND())) - 1;\r\n";
                        output += L"\t\t\tListView_SetColumnWidth(" + varName + L"->GetHWND(), columnIndex, " + width + L");\r\n";
                        output += L"\t\t}\r\n";
                    }
                }
            }
        }

        std::wstring showColumns = GetProperty(L"ShowColumns");
        if (showColumns == L"false" || showColumns == L"False" || showColumns == L"0") {
            output += L"\t\t" + varName + L"->HideColumns();\r\n";
        }

        output += GenerateElementSpecificEventHandlers(varName);

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"RightClick", L"DoubleClick", L"SelectionChanged" };
    }

    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        const std::vector<std::wstring> events = { L"RightClick", L"DoubleClick", L"SelectionChanged" };

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

class ListViewColumnGenerator : public GeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }
};