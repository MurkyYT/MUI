#pragma once

#include "./UIElement.h"
#include "./UIElementCollection.h"

namespace mui 
{
	enum GridUnitType
	{
		Pixel,
		Auto,
		Star
	};

	struct GridLength
	{
		GridLength() { value = 1; unit = GridUnitType::Star; }
		GridLength(double value) { this->value = value; unit = GridUnitType::Pixel; }
		GridLength(double value, GridUnitType type) { this->value = value; unit = type; }
		static GridLength Star() { return GridLength(1, GridUnitType::Star); };
		static GridLength Auto() { return GridLength(1, GridUnitType::Auto); };
		double value;
		GridUnitType unit;
	};

	struct RowDefinition 
	{
		RowDefinition(GridLength length) { height = length; }
		GridLength height;
		double actualHeight = 0;
	};

	struct ColumnDefinition 
	{
		ColumnDefinition(GridLength length) { width = length; }
		GridLength width;
		double actualWidth = 0;
	};

	class Grid : public UIElement
	{
	public:
		Grid();

		void AddColumn(ColumnDefinition definition) { m_columns.push_back(definition); }
		void AddRow(RowDefinition definition) { m_rows.push_back(definition); }

		void SetRow(const std::shared_ptr<UIElement>& element, size_t row);
		void SetRowSpan(const std::shared_ptr<UIElement>& element, size_t rowSpan);
		void SetColumn(const std::shared_ptr<UIElement>& element, size_t column);
		void SetColumnSpan(const std::shared_ptr<UIElement>& element, size_t columnSpan);

		std::vector<RowDefinition>& RowDefinitions();
		std::vector<ColumnDefinition>& ColumnDefinitions();

		void AddChild(const std::shared_ptr<UIElement>& element);
		void RemoveChild(const std::shared_ptr<UIElement>& element);

		size_t GetMinWidth() override;
		size_t GetMinHeight() override;
		size_t GetMaxWidth() override;
		size_t GetMaxHeight() override;

	private:
		struct GridPlacement
		{
			size_t row;
			size_t column;
			size_t rowSpan = 1;
			size_t columnSpan = 1;
		};

		size_t m_lastRequestedHeight = 0;
		size_t m_lastRequestedWidth = 0;
		UIElementCollection m_collection;
		std::vector<RowDefinition> m_rows;
		std::vector<ColumnDefinition> m_columns;
		std::unordered_map<UIElement*, GridPlacement> m_elementGridPlacement;

		void PerformLayout();
		void SetHWND(HWND hWnd) override;
		void SetParentHWND(HWND p_hWnd) override;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void CalculateRowHeights();
		void CalculateColumnWidths();
	};
}

