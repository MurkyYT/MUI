#pragma once
#include "IGenerator.h"

class NativeWindowHostGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::shared_ptr<mui::NativeWindowHost> host = std::make_shared<mui::NativeWindowHost>();
        ApplyCommonProperties(host);
        return host;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::NativeWindowHost> " : L"\t\t") +
            varName + L" = std::make_shared<mui::NativeWindowHost>();\r\n";

        output += GenerateCommonPropertySetters(varName);
        output += GenerateElementSpecificEventHandlers(varName);

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return { L"OnResize" };
    }

    std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const override {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        std::wstring onResize = GetProperty(L"OnResize");
        if (!onResize.empty()) {
            output += L"\t\t" + varName + L"->OnResize = std::bind(&" + className + L"::" + onResize +
                L", this, std::placeholders::_1, std::placeholders::_2);\r\n";
        }

        return output;
    }
};