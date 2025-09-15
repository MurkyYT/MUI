#pragma once

#include "IGenerator.h"

class LabelGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring text = GetProperty(L"Text");
        std::shared_ptr<mui::Label> label = std::make_shared<mui::Label>(text.c_str());
        ApplyCommonProperties(label);

        std::wstring textAlignment = GetProperty(L"TextAlignment");
        if (!textAlignment.empty())
        {
            if (textAlignment == L"Center")
                label->SetTextAlignment(mui::LayoutAlignment::Center);
            else if (textAlignment == L"Fill")
                label->SetTextAlignment(mui::LayoutAlignment::Fill);
            else if (textAlignment == L"Start")
                label->SetTextAlignment(mui::LayoutAlignment::Start);
            else if (textAlignment == L"End")
                label->SetTextAlignment(mui::LayoutAlignment::End);
        }

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                label->SetTextColor(color);
            }
        }

        return label;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Label> " : L"\t\t") + varName + L" = std::make_shared<mui::Label>(L\"" + EscapeString(text) + L"\");\r\n";

        output += GenerateCommonPropertySetters(varName);

        std::wstring textAlignment = GetProperty(L"TextAlignment");
        if (!textAlignment.empty())
            output += L"\t\t" + varName + L"->SetTextAlignment(mui::LayoutAlignment::" + textAlignment + L");\r\n";

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetTextColor(RGB(" +
                    std::to_wstring(r) + L", " +
                    std::to_wstring(g) + L", " +
                    std::to_wstring(b) + L"));\r\n";
            }
        }

        return output;
    }
};
