#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "IGenerator.h"

class GeneratorsManager {
public:
    using GeneratorFactory = std::function<std::shared_ptr<GeneratorBase>()>;

private:
    std::map<std::wstring, GeneratorFactory> generatorFactories;
    std::map<std::wstring, std::vector<std::wstring>> supportedProperties;

    std::vector<std::wstring> baseUIProperties = {
        L"Grid.Row",
        L"Grid.RowSpan",
        L"Grid.Column",
        L"Grid.ColumnSpan",
        L"Name",
        L"VerticalAlignment",
        L"HorizontalAlignment",
        L"Enabled",
        L"Border",
        L"BackgroundColor",
        L"KeyDown",
        L"KeyUp",
        L"LeftMouseDown",
        L"LeftMouseUp",
        L"LeftMouseDoubleClick",
        L"RightMouseDown",
        L"RightMouseUp",
        L"RightMouseDoubleClick",
        L"MouseMove",
        L"MouseEnter",
        L"MouseLeave"
    };

public:
    void RegisterGenerator(const std::wstring& elementName,
        GeneratorFactory factory,
        const std::vector<std::wstring>& properties = {}) {

        generatorFactories[elementName] = factory;

        std::vector<std::wstring> allProperties = properties;

        if (elementName != L"Window" && 
            elementName != L"Menu" && elementName != L"MenuItem" && elementName != L"MenuSeparator") {
            allProperties.insert(allProperties.begin(), baseUIProperties.begin(), baseUIProperties.end());

            std::vector<std::wstring> uniqueProperties;
            for (const auto& prop : allProperties) {
                if (std::find(uniqueProperties.begin(), uniqueProperties.end(), prop) == uniqueProperties.end()) {
                    uniqueProperties.push_back(prop);
                }
            }
            allProperties = uniqueProperties;
        }

        supportedProperties[elementName] = allProperties;
    }

    std::shared_ptr<GeneratorBase> CreateGenerator(const std::wstring& elementName) {
        auto it = generatorFactories.find(elementName);
        if (it != generatorFactories.end()) {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::wstring> GetAvailableElements() const {
        std::vector<std::wstring> elements;
        for (const auto& pair : generatorFactories) {
            elements.push_back(pair.first);
        }
        return elements;
    }

    std::vector<std::wstring> GetSupportedProperties(const std::wstring& elementName) const {
        auto it = supportedProperties.find(elementName);
        return (it != supportedProperties.end()) ? it->second : std::vector<std::wstring>{};
    }
};