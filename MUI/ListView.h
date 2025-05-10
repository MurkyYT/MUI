#pragma once

#include "./UIElement.h"

#include <Windows.h>
#include <commctrl.h>
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

		BOOL AddColumn(const wchar_t* title);

		BOOL AddItem(const std::shared_ptr<ListItem>& item);
		BOOL RemoveItem(const std::shared_ptr<ListItem>& item);
		BOOL RemoveItemByIndex(size_t index) { return RemoveItem(m_items[index]); }

		size_t ItemCount() { return m_items.size(); }

		int GetSelectedIndex();
		std::shared_ptr<ListItem> GetSelectedItem();

		size_t GetMinWidth() override;
		size_t GetMinHeight() override;
		size_t GetMaxWidth() override;
		size_t GetMaxHeight() override;

	private:
		void SetHWND(HWND hWnd) override;
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		UINT m_itemIndex = 0;
		UINT m_iconIndex = 0;
		UINT m_columnIndex = 0;
		std::vector<std::shared_ptr<ListItem>> m_items;
		std::vector<LVCOLUMN> m_columns;
		HIMAGELIST m_hLargeIcons = NULL;
		HIMAGELIST m_hSmallIcons = NULL;
	};

}