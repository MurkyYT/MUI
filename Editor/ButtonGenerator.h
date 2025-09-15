
#pragma once

#include "IGenerator.h"

class ButtonGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring text = GetProperty(L"Text");
        std::shared_ptr<mui::Button> button = std::make_shared<mui::Button>(text.c_str());

        ApplyCommonProperties(button);

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                button->SetTextColor(color);
            }
        }

        std::wstring hoverColor = GetProperty(L"HoverColor");
        if (!hoverColor.empty()) {
            COLORREF color = ParseColor(hoverColor);
            if (color != CLR_INVALID) {
                button->SetHoverColor(color);
            }
        }

        std::wstring pressedColor = GetProperty(L"PressedColor");
        if (!pressedColor.empty()) {
            COLORREF color = ParseColor(pressedColor);
            if (color != CLR_INVALID) {
                button->SetPressedColor(color);
            }
        }

        std::wstring borderColor = GetProperty(L"BorderColor");
        if (!borderColor.empty()) {
            COLORREF color = ParseColor(borderColor);
            if (color != CLR_INVALID) {
                button->SetBorderColor(color);
            }
        }

        return button;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Button> " : L"\t\t") +
            varName + L" = std::make_shared<mui::Button>(L\"" + EscapeString(text) + L"\");\r\n";

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

        std::wstring pressedColor = GetProperty(L"PressedColor");
        if (!pressedColor.empty()) {
            COLORREF color = ParseColor(pressedColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetPressedColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        std::wstring borderColor = GetProperty(L"BorderColor");
        if (!borderColor.empty()) {
            COLORREF color = ParseColor(borderColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetBorderColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        output += GenerateElementSpecificEventHandlers(varName);

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"OnClick" };
    }

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
};