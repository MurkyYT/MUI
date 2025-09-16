#pragma once
#include <string>
#include <vector>
#include <memory>
#include <set>

#include <MUI.h>

class GeneratorBase {
public:
    virtual std::wstring Generate() = 0;
    virtual std::shared_ptr<mui::UIElement> CreateUIElementPreview() { return NULL; }
    virtual std::shared_ptr<mui::Window> CreateWindowPreview() { return NULL; }

protected:
    std::wstring EscapeString(const std::wstring& input) const {
        std::wstring result = input;

        size_t pos = 0;
        while ((pos = result.find(L"\\", pos)) != std::wstring::npos) {
            result.replace(pos, 1, L"\\\\");
            pos += 2;
        }

        pos = 0;
        while ((pos = result.find(L"\n", pos)) != std::wstring::npos) {
            result.replace(pos, 1, L"\\n");
            pos += 2;
        }

        pos = 0;
        while ((pos = result.find(L"\r", pos)) != std::wstring::npos) {
            result.replace(pos, 1, L"\\r");
            pos += 2;
        }

        pos = 0;
        while ((pos = result.find(L"\t", pos)) != std::wstring::npos) {
            result.replace(pos, 1, L"\\t");
            pos += 2;
        }

        pos = 0;
        while ((pos = result.find(L"\"", pos)) != std::wstring::npos) {
            result.replace(pos, 1, L"\\\"");
            pos += 2;
        }

        return result;
    }

    std::vector<std::pair<std::wstring, std::wstring>> properties;
    std::vector<std::shared_ptr<GeneratorBase>> children;
    std::shared_ptr<GeneratorBase> m_parent;
    std::wstring elementName;
    size_t index = 0;

public:
    size_t GetIndex() const { return index; }
    void SetIndex(size_t index) {
        this->index = index;
    }

    void SetParent(std::shared_ptr<GeneratorBase> parent) {
        this->m_parent = parent;
    }

    void SetProperty(const std::wstring& name, const std::wstring& value) {
        properties.emplace_back(name, value);
    }

    std::wstring GetProperty(const std::wstring& name) const {
        for (const auto& property : properties) {
            if (property.first == name) return property.second;
        }
        return L"";
    }

    void AddChild(std::shared_ptr<GeneratorBase> child) {
        children.push_back(child);
    }

    const std::vector<std::shared_ptr<GeneratorBase>>& GetChildren() const {
        return children;
    }

    void SetElementName(const std::wstring& name) { elementName = name; }
    const std::wstring& GetElementName() const { return elementName; }

    std::wstring GenerateChildren() const {
        std::wstring result;
        for (const auto& child : children) {
            result += child->Generate();
        }
        return result;
    }

    std::wstring GetVariableName() const {
        std::wstring name = GetProperty(L"Name");
        if (!name.empty()) {
            return name;
        }
        return GetElementName() + std::to_wstring(GetIndex());
    }
};

class UIElementGeneratorBase : public GeneratorBase {
public:
    static std::wstring CollectAllMemberVariables(std::shared_ptr<GeneratorBase> generator) {
        std::wstring output;
        CollectMemberVariablesRecursive(generator, output);
        return output;
    }

    static void CollectMemberVariablesRecursive(std::shared_ptr<GeneratorBase> generator, std::wstring& output) {
        std::wstring name = generator->GetProperty(L"Name");
        if (!name.empty()) {
            std::wstring elementName = generator->GetElementName();
            output += L"\tstd::shared_ptr<mui::" + elementName + L"> " + name + L";\r\n";
        }

        for (const auto& child : generator->GetChildren()) {
            CollectMemberVariablesRecursive(child, output);
        }
    }
    static std::wstring CollectAllEventHandlerFunctions(std::shared_ptr<GeneratorBase> generator) {
        std::set<std::wstring> functions;
        CollectEventHandlerFunctionsRecursive(generator, functions);

        std::wstring output;
        for (const auto& func : functions) {
            output += L"\tvirtual void " + func + L"(const void* sender, mui::EventArgs_t* e) = 0;\r\n";
        }
        return output;
    }

    static void CollectEventHandlerFunctionsRecursive(std::shared_ptr<GeneratorBase> generator, std::set<std::wstring>& functions) {
        const std::vector<std::wstring> commonEvents = {
            L"KeyDown", L"KeyUp", L"LeftMouseDown", L"LeftMouseUp", L"LeftMouseDoubleClick",
            L"RightMouseDown", L"RightMouseUp", L"RightMouseDoubleClick",
            L"MouseMove", L"MouseEnter", L"MouseLeave"
        };

        for (const auto& eventProp : commonEvents) {
            std::wstring handlerName = generator->GetProperty(eventProp);
            if (!handlerName.empty()) {
                functions.insert(handlerName);
            }
        }

        if (auto uiGenerator = std::dynamic_pointer_cast<UIElementGeneratorBase>(generator)) {
            std::vector<std::wstring> specificEvents = uiGenerator->GetElementSpecificEvents();
            for (const auto& eventProp : specificEvents) {
                std::wstring handlerName = generator->GetProperty(eventProp);
                if (!handlerName.empty()) {
                    functions.insert(handlerName);
                }
            }
        }

        for (const auto& child : generator->GetChildren()) {
            CollectEventHandlerFunctionsRecursive(child, functions);
        }
    }
protected:
    std::wstring GenerateCommonPropertySetters(const std::wstring& varName) const {
        std::wstring output;

        std::wstring verticalAlignment = GetProperty(L"VerticalAlignment");
        if (!verticalAlignment.empty()) {
            output += L"\t\t" + varName + L"->SetVerticalAlignment(mui::LayoutAlignment::" + verticalAlignment + L");\r\n";
        }

        std::wstring horizontalAlignment = GetProperty(L"HorizontalAlignment");
        if (!horizontalAlignment.empty()) {
            output += L"\t\t" + varName + L"->SetHorizontalAlignment(mui::LayoutAlignment::" + horizontalAlignment + L");\r\n";
        }

        std::wstring enabled = GetProperty(L"Enabled");
        if (!enabled.empty()) {
            BOOL isEnabled = (enabled == L"true" || enabled == L"True" || enabled == L"1");
            output += L"\t\t" + varName + L"->SetEnabled(" + (isEnabled ? L"TRUE" : L"FALSE") + L");\r\n";
        }

        std::wstring border = GetProperty(L"Border");
        if (!border.empty()) {
            BOOL hasBorder = (border == L"true" || border == L"True" || border == L"1");
            output += L"\t\t" + varName + L"->SetBorder(" + (hasBorder ? L"TRUE" : L"FALSE") + L");\r\n";
        }

        std::wstring backgroundColor = GetProperty(L"BackgroundColor");
        if (!backgroundColor.empty()) {
            COLORREF color = ParseColor(backgroundColor);
            if (color != CLR_INVALID) {
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                output += L"\t\t" + varName + L"->SetBackgroundColor(RGB(" +
                    std::to_wstring(r) + L", " + std::to_wstring(g) + L", " + std::to_wstring(b) + L"));\r\n";
            }
        }

        output += GenerateCommonEventHandlers(varName);

        return output;
    }

    std::wstring GenerateCommonEventHandlers(const std::wstring& varName) const {
        std::wstring output;
        std::wstring className = GetCurrentClassName();

        const std::vector<std::wstring> commonEvents = {
            L"KeyDown", L"KeyUp", L"LeftMouseDown", L"LeftMouseUp", L"LeftMouseDoubleClick",
            L"RightMouseDown", L"RightMouseUp", L"RightMouseDoubleClick",
            L"MouseMove", L"MouseEnter", L"MouseLeave"
        };

        for (const auto& eventName : commonEvents) {
            std::wstring handlerName = GetProperty(eventName);
            if (!handlerName.empty()) {
                output += L"\t\t" + varName + L"->" + eventName + L" = std::bind(&" + className + L"::" + handlerName +
                    L", this, std::placeholders::_1, std::placeholders::_2);\r\n";
            }
        }

        return output;
    }

    virtual std::wstring GenerateElementSpecificEventHandlers(const std::wstring& varName) const {
        return L"";
    }

    virtual std::vector<std::wstring> GetElementSpecificEvents() const {
        return {};
    }

    std::wstring GetCurrentClassName() const {
        std::shared_ptr<GeneratorBase> current = GetRootGenerator();
        if (current && current->GetElementName() == L"Window") {
            std::wstring className = current->GetProperty(L"Class");
            if (!className.empty()) {
                return className + L"Base";
            }
        }
        return L"MainWindowBase";
    }

    std::shared_ptr<GeneratorBase> GetRootGenerator() const {
        return s_rootGenerator;
    }

    template<typename T>
    void ApplyCommonProperties(std::shared_ptr<T> element) const {
        std::wstring verticalAlignment = GetProperty(L"VerticalAlignment");
        if (!verticalAlignment.empty()) {
            if (verticalAlignment == L"Center")
                element->SetVerticalAlignment(mui::LayoutAlignment::Center);
            else if (verticalAlignment == L"Fill")
                element->SetVerticalAlignment(mui::LayoutAlignment::Fill);
            else if (verticalAlignment == L"Start")
                element->SetVerticalAlignment(mui::LayoutAlignment::Start);
            else if (verticalAlignment == L"End")
                element->SetVerticalAlignment(mui::LayoutAlignment::End);
        }

        std::wstring horizontalAlignment = GetProperty(L"HorizontalAlignment");
        if (!horizontalAlignment.empty()) {
            if (horizontalAlignment == L"Center")
                element->SetHorizontalAlignment(mui::LayoutAlignment::Center);
            else if (horizontalAlignment == L"Fill")
                element->SetHorizontalAlignment(mui::LayoutAlignment::Fill);
            else if (horizontalAlignment == L"Start")
                element->SetHorizontalAlignment(mui::LayoutAlignment::Start);
            else if (horizontalAlignment == L"End")
                element->SetHorizontalAlignment(mui::LayoutAlignment::End);
        }

        std::wstring enabled = GetProperty(L"Enabled");
        if (!enabled.empty()) {
            BOOL isEnabled = (enabled == L"true" || enabled == L"True" || enabled == L"1");
            element->SetEnabled(isEnabled);
        }

        std::wstring border = GetProperty(L"Border");
        if (!border.empty()) {
            BOOL hasBorder = (border == L"true" || border == L"True" || border == L"1");
            element->SetBorder(hasBorder);
        }

        std::wstring backgroundColor = GetProperty(L"BackgroundColor");
        if (!backgroundColor.empty()) {
            COLORREF color = ParseColor(backgroundColor);
            if (color != CLR_INVALID) {
                element->SetBackgroundColor(color);
            }
        }
    }

    COLORREF ParseColor(const std::wstring& colorStr) const {
        if (colorStr.empty()) return CLR_INVALID;

        if (colorStr.find(L',') != std::wstring::npos || colorStr.find(L"rgb") != std::wstring::npos) {
            std::wstring cleaned = colorStr;

            size_t start = cleaned.find(L"rgb(");
            if (start != std::wstring::npos) {
                cleaned = cleaned.substr(start + 4);
                size_t end = cleaned.find(L')');
                if (end != std::wstring::npos) {
                    cleaned = cleaned.substr(0, end);
                }
            }

            std::vector<int> values;
            std::wstring current;
            for (wchar_t c : cleaned) {
                if (c == L',' || c == L' ') {
                    if (!current.empty()) {
                        values.push_back(_wtoi(current.c_str()));
                        current.clear();
                    }
                }
                else if (c >= L'0' && c <= L'9') {
                    current += c;
                }
            }
            if (!current.empty()) {
                values.push_back(_wtoi(current.c_str()));
            }

            if (values.size() == 3) {
                return RGB(values[0], values[1], values[2]);
            }
        }

        if (colorStr[0] == L'#' || colorStr.substr(0, 2) == L"0x") {
            std::wstring hexStr = colorStr;
            if (hexStr[0] == L'#') hexStr = hexStr.substr(1);
            if (hexStr.substr(0, 2) == L"0x") hexStr = hexStr.substr(2);

            if (hexStr.length() == 6) {
                unsigned long value = wcstoul(hexStr.c_str(), nullptr, 16);
                return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
            }
        }

        if (colorStr == L"Red" || colorStr == L"red") return RGB(255, 0, 0);
        if (colorStr == L"Green" || colorStr == L"green") return RGB(0, 255, 0);
        if (colorStr == L"Blue" || colorStr == L"blue") return RGB(0, 0, 255);
        if (colorStr == L"White" || colorStr == L"white") return RGB(255, 255, 255);
        if (colorStr == L"Black" || colorStr == L"black") return RGB(0, 0, 0);
        if (colorStr == L"Yellow" || colorStr == L"yellow") return RGB(255, 255, 0);
        if (colorStr == L"Gray" || colorStr == L"gray") return RGB(128, 128, 128);

        return CLR_INVALID;
    }

public:
    static std::shared_ptr<GeneratorBase> s_rootGenerator;

    virtual std::wstring Generate() override = 0;
    virtual std::shared_ptr<mui::UIElement> CreateUIElementPreview() override = 0;
};