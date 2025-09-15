#pragma once

#include "IGenerator.h"

class StackLayoutGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::wstring orientationStr = GetProperty(L"Orientation");
        mui::StackLayoutOrientation orientation = mui::StackLayoutOrientation::Vertical;

        if (orientationStr == L"Horizontal")
            orientation = mui::StackLayoutOrientation::Horizontal;

        std::shared_ptr<mui::StackLayout> stackLayout = std::make_shared<mui::StackLayout>(orientation);

        ApplyCommonProperties(stackLayout);

        for (const auto& child : GetChildren()) {
            std::shared_ptr<mui::UIElement> childElement = child->CreateUIElementPreview();
            if (childElement) {
                stackLayout->Children().Add(childElement);
            }
        }

        return stackLayout;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();
        std::wstring orientationStr = GetProperty(L"Orientation");

        std::wstring orientation = L"mui::StackLayoutOrientation::Vertical";
        if (orientationStr == L"Horizontal")
            orientation = L"mui::StackLayoutOrientation::Horizontal";

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::StackLayout> " : L"\t\t") +
            varName + L" = std::make_shared<mui::StackLayout>(" + orientation + L");\r\n";

        output += GenerateCommonPropertySetters(varName);

        output += GenerateChildren();

        for (const auto& child : GetChildren()) {
            std::wstring childVarName = child->GetVariableName();
            output += L"\t\t" + varName + L"->Children().Add(" + childVarName + L");\r\n";
        }

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return {};
    }
};