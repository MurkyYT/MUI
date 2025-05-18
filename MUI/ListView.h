#pragma once

#include "./UIElement.h"
#include "./Events.h"

#include <Windows.h>
#include <string>
#include <memory>
#include <vector>

namespace mui
{

	class ListItem
	{
		friend class ListView;
	public:
		ListItem(std::vector<std::wstring> values, HICON icon = NULL)
		{
			m_values = values;
			m_hIcon = icon;
		}
	private:
		std::vector<std::wstring> m_values;
		HICON m_hIcon = NULL;
		int m_index = -1;
		int m_iconIndex = -1;
	};

	class ListView : public UIElement
	{
	public:
		ListView(int x = 0, int y = 0, int width = 0, int height = 0);

		BOOL HideColumns();
		BOOL ShowColumns();

		BOOL AddColumn(const wchar_t* title);

		BOOL AddItem(const std::shared_ptr<ListItem>& item);
		BOOL RemoveItem(const std::shared_ptr<ListItem>& item);
		BOOL RemoveItemByIndex(size_t index) { return RemoveItem(m_items[index]); }

		size_t ItemCount() { return m_items.size(); }

		int GetSelectedIndex();
		std::shared_ptr<ListItem> GetSelectedItem();

		void SetTextColor(COLORREF color);
		void SetBackgroundColor(COLORREF color) override;
		void SetSeparatorColor(COLORREF color);
		void SetHoverColor(COLORREF color);

		EventCallback_t RightClick{ NULL };
		EventCallback_t DoubleClick{ NULL };
		EventCallback_t SelectionChanged{ NULL };

	private:
		void SetHWND(HWND hWnd) override;
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		UINT m_itemIndex = 0;
		UINT m_iconIndex = 0;
		UINT m_columnIndex = 0;
		BOOL m_columnsVisible = TRUE;
		int m_hotHeaderColumn = -1;

		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_seperatorColor = RGB(229, 229, 229);
		COLORREF m_hoverColor = RGB(217, 235, 249);

		std::vector<std::shared_ptr<ListItem>> m_items;
		std::vector<LVCOLUMN> m_columns;
		HIMAGELIST m_hLargeIcons = NULL;
		HIMAGELIST m_hSmallIcons = NULL;

		static LRESULT CALLBACK HeaderSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
			LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	};

}