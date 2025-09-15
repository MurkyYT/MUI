#pragma once

#include "IGenerator.h"

class GridGenerator : public UIElementGeneratorBase {
public:
    std::shared_ptr<mui::UIElement> CreateUIElementPreview() override
    {
        std::shared_ptr<mui::Grid> grid = std::make_shared<mui::Grid>();

        ApplyCommonProperties(grid);

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"Grid.RowDefinitions") {
                for (const auto& rowDef : child->GetChildren()) {
                    if (rowDef->GetElementName() == L"RowDefinition") {
                        mui::RowDefinition definition = CreateRowDefinition(rowDef);
                        grid->AddRow(definition);
                    }
                }
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"Grid.ColumnDefinitions") {
                for (const auto& colDef : child->GetChildren()) {
                    if (colDef->GetElementName() == L"ColumnDefinition") {
                        mui::ColumnDefinition definition = CreateColumnDefinition(colDef);
                        grid->AddColumn(definition);
                    }
                }
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() != L"Grid.RowDefinitions" && child->GetElementName() != L"Grid.ColumnDefinitions") {
                std::shared_ptr<mui::UIElement> childElement = child->CreateUIElementPreview();
                if (childElement) {
                    grid->AddChild(childElement);

                    std::wstring row = child->GetProperty(L"Grid.Row");
                    if (!row.empty()) {
                        grid->SetRow(childElement, _wtoi(row.c_str()));
                    }

                    std::wstring column = child->GetProperty(L"Grid.Column");
                    if (!column.empty()) {
                        grid->SetColumn(childElement, _wtoi(column.c_str()));
                    }

                    std::wstring rowSpan = child->GetProperty(L"Grid.RowSpan");
                    if (!rowSpan.empty()) {
                        grid->SetRowSpan(childElement, _wtoi(rowSpan.c_str()));
                    }

                    std::wstring columnSpan = child->GetProperty(L"Grid.ColumnSpan");
                    if (!columnSpan.empty()) {
                        grid->SetColumnSpan(childElement, _wtoi(columnSpan.c_str()));
                    }
                }
            }
        }

        return grid;
    }

    std::wstring Generate() override
    {
        std::wstring output;
        std::wstring varName = GetVariableName();

        output += (GetProperty(L"Name").empty() ? L"\t\tstd::shared_ptr<mui::Grid> " : L"\t\t") +
            varName + L" = std::make_shared<mui::Grid>();\r\n";

        output += GenerateCommonPropertySetters(varName);

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"Grid.RowDefinitions") {
                for (const auto& rowDef : child->GetChildren()) {
                    if (rowDef->GetElementName() == L"RowDefinition") {
                        output += GenerateRowDefinition(varName, rowDef);
                    }
                }
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() == L"Grid.ColumnDefinitions") {
                for (const auto& colDef : child->GetChildren()) {
                    if (colDef->GetElementName() == L"ColumnDefinition") {
                        output += GenerateColumnDefinition(varName, colDef);
                    }
                }
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() != L"Grid.RowDefinitions" && child->GetElementName() != L"Grid.ColumnDefinitions") {
                output += child->Generate();
            }
        }

        for (const auto& child : GetChildren()) {
            if (child->GetElementName() != L"Grid.RowDefinitions" && child->GetElementName() != L"Grid.ColumnDefinitions") {
                std::wstring childVarName = child->GetVariableName();
                output += L"\t\t" + varName + L"->AddChild(" + childVarName + L");\r\n";

                std::wstring row = child->GetProperty(L"Grid.Row");
                if (!row.empty()) {
                    output += L"\t\t" + varName + L"->SetRow(" + childVarName + L", " + row + L");\r\n";
                }

                std::wstring column = child->GetProperty(L"Grid.Column");
                if (!column.empty()) {
                    output += L"\t\t" + varName + L"->SetColumn(" + childVarName + L", " + column + L");\r\n";
                }

                std::wstring rowSpan = child->GetProperty(L"Grid.RowSpan");
                if (!rowSpan.empty()) {
                    output += L"\t\t" + varName + L"->SetRowSpan(" + childVarName + L", " + rowSpan + L");\r\n";
                }

                std::wstring columnSpan = child->GetProperty(L"Grid.ColumnSpan");
                if (!columnSpan.empty()) {
                    output += L"\t\t" + varName + L"->SetColumnSpan(" + childVarName + L", " + columnSpan + L");\r\n";
                }
            }
        }

        return output;
    }

protected:
    std::vector<std::wstring> GetElementSpecificEvents() const override {
        return {};
    }

private:
    mui::RowDefinition CreateRowDefinition(std::shared_ptr<GeneratorBase> rowDef) const {
        std::wstring height = rowDef->GetProperty(L"Height");
        return mui::RowDefinition(ParseGridLength(height));
    }

    mui::ColumnDefinition CreateColumnDefinition(std::shared_ptr<GeneratorBase> colDef) const {
        std::wstring width = colDef->GetProperty(L"Width");
        return mui::ColumnDefinition(ParseGridLength(width));
    }

    std::wstring GenerateRowDefinition(const std::wstring& gridVarName, std::shared_ptr<GeneratorBase> rowDef) const {
        std::wstring height = rowDef->GetProperty(L"Height");
        std::wstring gridLength = GenerateGridLength(height);
        return L"\t\t" + gridVarName + L"->AddRow(mui::RowDefinition(" + gridLength + L"));\r\n";
    }

    std::wstring GenerateColumnDefinition(const std::wstring& gridVarName, std::shared_ptr<GeneratorBase> colDef) const {
        std::wstring width = colDef->GetProperty(L"Width");
        std::wstring gridLength = GenerateGridLength(width);
        return L"\t\t" + gridVarName + L"->AddColumn(mui::ColumnDefinition(" + gridLength + L"));\r\n";
    }

    mui::GridLength ParseGridLength(const std::wstring& value) const {
        if (value.empty() || value == L"*") {
            return mui::GridLength::Star();
        }

        if (value == L"Auto" || value == L"auto") {
            return mui::GridLength::Auto();
        }

        if (value.back() == L'*') {
            std::wstring numPart = value.substr(0, value.length() - 1);
            if (numPart.empty()) {
                return mui::GridLength::Star();
            }
            double starValue = _wtof(numPart.c_str());
            return mui::GridLength(starValue, mui::GridUnitType::Star);
        }

        double pixelValue = _wtof(value.c_str());
        if (pixelValue > 0) {
            return mui::GridLength(pixelValue, mui::GridUnitType::Pixel);
        }

        return mui::GridLength::Star();
    }

    std::wstring GenerateGridLength(const std::wstring& value) const {
        if (value.empty() || value == L"*") {
            return L"mui::GridLength::Star()";
        }

        if (value == L"Auto" || value == L"auto") {
            return L"mui::GridLength::Auto()";
        }

        if (value.back() == L'*') {
            std::wstring numPart = value.substr(0, value.length() - 1);
            if (numPart.empty()) {
                return L"mui::GridLength::Star()";
            }
            return L"mui::GridLength(" + numPart + L", mui::GridUnitType::Star)";
        }

        double pixelValue = _wtof(value.c_str());
        if (pixelValue > 0) {
            return L"mui::GridLength(" + value + L", mui::GridUnitType::Pixel)";
        }

        return L"mui::GridLength::Star()";
    }
};

class RowDefinitionsGenerator : public GeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }
};

class RowDefinitionGenerator : public GeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }
};

class ColumnDefinitionsGenerator : public GeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }
};

class ColumnDefinitionGenerator : public GeneratorBase {
public:
    std::wstring Generate() override {
        return L"";
    }
};