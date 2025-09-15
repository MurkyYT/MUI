#pragma once

#include "IGenerator.h"

class CheckBoxGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring text = GetProperty(L"Text");
        std::shared_ptr<mui::CheckBox> checkBox = std::make_shared<mui::CheckBox>(text.c_str());

        ApplyCommonProperties(checkBox);

        std::wstring checked = GetProperty(L"Checked");
        if (!checked.empty()) {
            BOOL isChecked = (checked == L"true" || checked == L"True" || checked == L"1");
            checkBox->SetChecked(isChecked);
        }

        std::wstring textColor = GetProperty(L"TextColor");
        if (!textColor.empty()) {
            COLORREF color = ParseColor(textColor);
            if (color != CLR_INVALID) {
                checkBox->SetTextColor(color);
            }
        }

        return checkBox;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring text = GetProperty(L"Text");

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::CheckBox> " : L"\t\t") +
            varName + L" = std::make_shared<mui::CheckBox>(L\"" + EscapeString(text) + L"\");\r\n";

        output += GenerateCommonPropertySetters(varName);

        std::wstring checked = GetProperty(L"Checked");
        if (!checked.empty()) {
            BOOL isChecked = (checked == L"true" || checked == L"True" || checked == L"1");
            output += L"\t\t" + varName + L"->SetChecked(" + (isChecked ? L"TRUE" : L"FALSE") + L");\r\n";
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
        return { L"StateChanged" };
    }

    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        std::wstring stateChanged = GetProperty(L"StateChanged");
        if (!stateChanged.empty()) {
            output += L"\t\t" + varName + L"->StateChanged = std::bind(&" + className + L"::" + stateChanged +
                L", this, std::placeholders::_1, std::placeholders::_2);\r\n";
        }

        return output;
    }
};