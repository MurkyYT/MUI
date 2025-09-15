#pragma once

#include "IGenerator.h"

class EntryGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring text = GetProperty(L"Text");
        std::shared_ptr<mui::Entry> entry = std::make_shared<mui::Entry>(text.c_str());

        ApplyCommonProperties(entry);

        std::wstring placeholder = GetProperty(L"Placeholder");
        if (!placeholder.empty()) {
            entry->SetPlaceholder(placeholder);
        }

        std::wstring textAlignment = GetProperty(L"TextAlignment");
        if (!textAlignment.empty()) {
            if (textAlignment == L"Center")
                entry->SetTextAlignment(mui::LayoutAlignment::Center);
            else if (textAlignment == L"Fill")
                entry->SetTextAlignment(mui::LayoutAlignment::Fill);
            else if (textAlignment == L"Start")
                entry->SetTextAlignment(mui::LayoutAlignment::Start);
            else if (textAlignment == L"End")
                entry->SetTextAlignment(mui::LayoutAlignment::End);
        }

        std::wstring multiline = GetProperty(L"Multiline");
        if (!multiline.empty()) {
            BOOL isMultiline = (multiline == L"true" || multiline == L"True" || multiline == L"1");
            entry->SetMultiline(isMultiline);
        }

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                entry->SetTextColor(color);
            }
        }

        return entry;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Entry> " : L"\t\t") +
            varName + L" = std::make_shared<mui::Entry>(L\"" + EscapeString(text) + L"\");\r\n";

        output += GenerateCommonPropertySetters(varName);

        std::wstring placeholder = GetProperty(L"Placeholder");
        if (!placeholder.empty()) {
            output += L"\t\t" + varName + L"->SetPlaceholder(L\"" + EscapeString(placeholder) + L"\");\r\n";
        }

        std::wstring textAlignment = GetProperty(L"TextAlignment");
        if (!textAlignment.empty()) {
            output += L"\t\t" + varName + L"->SetTextAlignment(mui::LayoutAlignment::" + textAlignment + L");\r\n";
        }

        std::wstring multiline = GetProperty(L"Multiline");
        if (!multiline.empty()) {
            BOOL isMultiline = (multiline == L"true" || multiline == L"True" || multiline == L"1");
            output += L"\t\t" + varName + L"->SetMultiline(" + (isMultiline ? L"TRUE" : L"FALSE") + L");\r\n";
        }

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

        output += GenerateElementSpecificEventHandlers(varName);

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"TextChanged", L"NewLine", L"CharPressed", L"Completed", L"Save" };
    }

    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        const std::vector<std::wstring> events = { L"TextChanged", L"NewLine", L"CharPressed", L"Completed", L"Save" };

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