#pragma once
#include "IGenerator.h"

class SectionGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring text = GetProperty(L"Text");
        std::shared_ptr<mui::Section> section = std::make_shared<mui::Section>(text);

        ApplyCommonProperties(section);

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                section->SetTextColor(color);
            }
        }

        std::wstring expandButtonColor = GetProperty(L"ExpandButtonColor");
        if (!expandButtonColor.empty()) {
            COLORREF color = ParseColor(expandButtonColor);
            if (color != CLR_INVALID) {
                section->SetExpandButtonColor(color);
            }
        }

        for (const auto& child : GetChildren()) {
            std::shared_ptr<mui::UIElement> childElement = child->CreateUIElementPreview();
            if (childElement) {
                section->SetContent(childElement);
                break;
            }
        }

        return section;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Section> " : L"\t\t") +
            varName + L" = std::make_shared<mui::Section>(L\"" + EscapeString(text) + L"\");\r\n";

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

        output += GenerateChildren();

        for (const auto& child : GetChildren()) {
            std::wstring childVarName = child->GetVariableName();
            output += L"\t\t" + varName + L"->SetContent(" + childVarName + L");\r\n";
            break;
        }

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return {};
    }
};