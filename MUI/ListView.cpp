#include "ListView.h"

mui::ListView::ListView(int x, int y, int width, int height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_class = WC_LISTVIEW;
	m_name = L"";
	m_style = WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | WS_CLIPSIBLINGS | LVS_SINGLESEL;
	m_exStyle = TVS_EX_DOUBLEBUFFER;

	m_hLargeIcons = ImageList_Create(32,
		32,
		ILC_COLOR32, 3, 0);
	m_hSmallIcons = ImageList_Create(32,
		32,
		ILC_COLOR32, 3, 0);
}

void mui::ListView::UpdateIdealSize()
{
	DWORD res = ListView_ApproximateViewRect(m_hWnd, -1, -1, -1);
	m_idealSize = { LOWORD(res), HIWORD(res) };
}
BOOL mui::ListView::HideColumns()
{
	if (!m_hWnd)
	{
		m_columnsVisible = FALSE;
		return FALSE;
	}

	HWND hHeader = ListView_GetHeader(m_hWnd);
	LONG_PTR styles = GetWindowLongPtr(hHeader, GWL_STYLE);
	BOOL res = SetWindowLongPtr(hHeader, GWL_STYLE, styles | HDS_HIDDEN) > 0;
	UpdateIdealSize();
	return res;
}

BOOL mui::ListView::ShowColumns()
{
	if (!m_hWnd)
	{
		m_columnsVisible = TRUE;
		return TRUE;
	}

	HWND hHeader = ListView_GetHeader(m_hWnd);
	LONG_PTR styles = GetWindowLongPtr(hHeader, GWL_STYLE);
	BOOL res = SetWindowLongPtr(hHeader, GWL_STYLE, styles ^ HDS_HIDDEN) > 0;
	UpdateIdealSize();
	return res;
}

mui::UIElement::EventHandlerResult mui::ListView::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
			if (this->RightClick)
				RightClick(this, { uMsg,wParam,lParam });
			break;
		case NM_DBLCLK:
			if (this->DoubleClick)
				DoubleClick(this, { uMsg,wParam,lParam });
			break;
		}
	}
	break;
	}

	return { FALSE,NULL };
}

int mui::ListView::GetSelectedIndex()
{
	return ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
}

std::shared_ptr<mui::ListItem> mui::ListView::GetSelectedItem()
{
	int index = GetSelectedIndex();
	if (index > 0)
		return m_items[index];

	return NULL;
}

BOOL mui::ListView::AddColumn(const wchar_t* title)
{
	LVCOLUMN lvc{};
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = m_columnIndex++;
	lvc.pszText = (LPWSTR)title;
	lvc.cx = LVSCW_AUTOSIZE;
	lvc.fmt = LVCFMT_LEFT;

	m_columns.push_back(lvc);

	if (m_hWnd) 
	{
		UpdateIdealSize();
		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
		return ListView_InsertColumn(m_hWnd, lvc.iSubItem, &lvc);
	}

	return TRUE;
}

BOOL mui::ListView::RemoveItem(const std::shared_ptr<ListItem>& item)
{
	auto it = std::find(m_items.begin(), m_items.end(), item);

	if (it == m_items.end())
		return FALSE;

	if (m_hWnd)
	{
		ListView_DeleteItem(m_hWnd, item->m_index);
		BOOL hasImage = item->m_iconIndex != -1;

		if (hasImage) 
		{
			m_iconIndex--;
			ImageList_Remove(m_hLargeIcons, item->m_iconIndex);
			ImageList_Remove(m_hSmallIcons, item->m_iconIndex);
		}

		for (size_t i = item->m_index + 1; i < m_items.size(); i++)
		{
			m_items[i]->m_index--;
			LVITEM item{};
			item.iItem = (int)i - 1;
			item.mask = LVIF_IMAGE;
			ListView_GetItem(m_hWnd, &item);
			if(hasImage && m_items[i]->m_iconIndex != -1) item.iImage--;
			ListView_SetItem(m_hWnd, &item);
			if(hasImage && m_items[i]->m_iconIndex != -1) m_items[i]->m_iconIndex--;
		}

		UpdateIdealSize();

		InvalidateRect(m_hWnd, NULL, TRUE);

		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);

		m_itemIndex--;

		m_items.erase(it);

		return GetLastError() == 0;
	}

	m_items.erase(it);

	return TRUE;
}

BOOL mui::ListView::AddItem(const std::shared_ptr<ListItem>& item) 
{
	m_items.push_back(item);

	if (m_hWnd) 
	{
		LVITEM lvi{};
		BOOL suc = TRUE;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = m_itemIndex++;
		item->m_index = lvi.iItem;
		if (item->m_hIcon)
		{
			ImageList_AddIcon(m_hLargeIcons, item->m_hIcon);
			ImageList_AddIcon(m_hSmallIcons, item->m_hIcon);
			ListView_SetImageList(m_hWnd, m_hLargeIcons, LVSIL_NORMAL);
			ListView_SetImageList(m_hWnd, m_hSmallIcons, LVSIL_SMALL);
			lvi.iImage = m_iconIndex++;
			item->m_iconIndex = lvi.iImage;
		}
		else
			lvi.iImage = -1;

		lvi.iSubItem = 0;
		lvi.pszText = (LPWSTR)L"";
		ListView_InsertItem(m_hWnd, (LPARAM)&lvi);
		UINT index = 0;
		for (std::wstring value : item->m_values)
		{
			lvi.iSubItem = index;
			lvi.pszText = (LPWSTR)value.c_str();
			ListView_SetItem(m_hWnd, (LPARAM)&lvi);
			ListView_SetColumnWidth(m_hWnd, index, LVSCW_AUTOSIZE);
			index++;
		}

		UpdateIdealSize();

		InvalidateRect(m_hWnd, NULL, TRUE);

		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);

		return GetLastError() == 0;
	}
	
	return TRUE;
}

void mui::ListView::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;

	if (!m_columnsVisible)
		HideColumns();

	for (const LVCOLUMN& column : m_columns)
		ListView_InsertColumn(m_hWnd, column.iSubItem, &column);

	for (const std::shared_ptr<ListItem>& item : m_items)
	{
		LVITEM lvi{};
		BOOL suc = TRUE;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = m_itemIndex++;
		item->m_index = lvi.iItem;
		if (item->m_hIcon)
		{
			ImageList_AddIcon(m_hLargeIcons, item->m_hIcon);
			ImageList_AddIcon(m_hSmallIcons, item->m_hIcon);
			ListView_SetImageList(m_hWnd, m_hLargeIcons, LVSIL_NORMAL);
			ListView_SetImageList(m_hWnd, m_hSmallIcons, LVSIL_SMALL);
			lvi.iImage = m_iconIndex++;
			item->m_iconIndex = lvi.iImage;
		}
		else
			lvi.iImage = -1;

		lvi.iSubItem = 0;
		lvi.pszText = (LPWSTR)L"";
		ListView_InsertItem(m_hWnd, (LPARAM)&lvi);
		UINT index = 0;
		for (std::wstring value : item->m_values)
		{
			lvi.iSubItem = index;
			lvi.pszText = (LPWSTR)value.c_str();
			ListView_SetItem(m_hWnd, (LPARAM)&lvi);
			ListView_SetColumnWidth(m_hWnd, index, LVSCW_AUTOSIZE);
			index++;
		}
	}
	
	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
}