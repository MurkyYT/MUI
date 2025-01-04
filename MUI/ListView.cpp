#include "MUI.h"

namespace MUI
{
	ListView::~ListView()
	{
		this->Clear();
	}
	ListView::ListView(int x,int y,int width, int height, BOOL autoWidth)
	{
		this->columnIndex = 0;
		this->itemIndex = 0;
		this->type = MUI::UIType::UIListView;
		this->m_ClassName = WC_LISTVIEW;
		this->m_WindowName = L"";
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->autoWidth = autoWidth;
		this->style = WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
			 WS_EX_CLIENTEDGE | WS_CLIPSIBLINGS;
		this->hLarge = ImageList_Create(32,
			32,
			ILC_COLOR32, 3, 0);
		this->hSmall = ImageList_Create(32,
			32,
			ILC_COLOR32, 3, 0);
	}
	void ListView::ClearIcons()
	{
		ImageList_RemoveAll(this->hLarge);
		ImageList_RemoveAll(this->hSmall);
		if (hLarge)
			ImageList_Destroy(hLarge);
		if (hSmall)
			ImageList_Destroy(hSmall);
		this->hLarge = ImageList_Create(32,
			32,
			ILC_COLOR32, 3, 0);
		this->hSmall = ImageList_Create(32,
			32,
			ILC_COLOR32, 3, 0);
	}
	void ListView::DeleteIconAt(int i)
	{
		ImageList_Remove(this->hLarge, i);
		ImageList_Remove(this->hSmall, i);
	}
	void ListView::DeleteItemAt(int i)
	{
		ListView_DeleteItem(this->handle,i);
		delete m_Items[i];
		this->m_Items.erase(this->m_Items.begin() + i);
		this->itemIndex--;
	}
	void ListView::ClearItems()
	{
		ListView_DeleteAllItems(this->handle);
		for (size_t i = 0; i < m_Items.size(); i++)
			delete m_Items[i];
		this->m_Items.erase(this->m_Items.begin(), this->m_Items.end());
		this->itemIndex = 0;
	}
	void ListView::ClearColumns()
	{
		for (size_t i = 0; i < this->columnIndex; i++)
			ListView_DeleteColumn(this->handle, i);
		this->columnIndex = 0;
	}
	void ListView::Clear()
	{
		this->ClearColumns();
		this->ClearIcons();
		this->ClearItems();
	}
	int ListView::GetSelectedIndex() 
	{
		return ListView_GetNextItem(this->handle, -1, LVNI_SELECTED);
	}
	ListItem* ListView::GetItemAt(int i)
	{
		return this->m_Items[i];
	}
	ListItem* ListView::GetSelected()
	{
		int iPos = ListView_GetNextItem(this->handle, -1, LVNI_SELECTED);
		if(iPos != -1)
			return this->m_Items[iPos];
		return NULL;
	}
	BOOL ListView::AddIcon(HICON icon)
	{
		ImageList_AddIcon(this->hSmall, icon);
		ImageList_AddIcon(this->hLarge, icon);
		ListView_SetImageList(this->handle, this->hLarge, LVSIL_NORMAL);
		ListView_SetImageList(this->handle, this->hSmall, LVSIL_SMALL);
		return GetLastError() == 0;
	}
	BOOL ListView::AddColumn(const wchar_t* title,int length)
	{
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.iSubItem = this->columnIndex;
		lvc.pszText = (LPWSTR)title;
		lvc.cx = length;
		lvc.fmt = LVCFMT_LEFT;
		return ListView_InsertColumn(this->handle, this->columnIndex++, &lvc);
	}
	BOOL ListView::AddItem(ListItem* item)
	{
		LVITEM lvi;
		BOOL suc = TRUE;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = this->itemIndex++;
		if (item->imageIndex > -1)
			lvi.iImage = item->imageIndex;
		lvi.iSubItem = 0;
		lvi.pszText = (LPWSTR)L"";
		ListView_InsertItem(this->handle, (LPARAM)&lvi);
		UINT index = 0;
		for(std::wstring value : item->values)
		{
			lvi.iSubItem = index;
			lvi.pszText = (LPWSTR)value.c_str();
			ListView_SetItem(this->handle, (LPARAM)&lvi);
			if(this->autoWidth)
				ListView_SetColumnWidth(this->handle, index, LVSCW_AUTOSIZE);
			index++;
		}
		this->m_Items.push_back(item);
		return GetLastError() == 0;
	}
	void ListView::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	}
}