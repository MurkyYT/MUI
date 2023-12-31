#include "MUI.h"

namespace MUI
{
	ListView::ListView(int x,int y,int width, int height)
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
		this->style = WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
			 WS_EX_CLIENTEDGE;
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
	}
	void ListView::DeleteIconAt(int i)
	{
		ImageList_Remove(this->hLarge, i);
		ImageList_Remove(this->hSmall, i);
	}
	void ListView::DeleteItemAt(int i)
	{
		ListView_DeleteItem(this->handle,i);
		this->m_Items.erase(this->m_Items.begin() + i);
		this->itemIndex--;
	}
	void ListView::ClearItems()
	{
		ListView_DeleteAllItems(this->handle);
		this->m_Items.erase(this->m_Items.begin(), this->m_Items.end());
		this->itemIndex = 0;
	}
	int ListView::GetSelectedIndex() 
	{
		return ListView_GetNextItem(this->handle, -1, LVNI_SELECTED);
	}
	ListItem* ListView::GetItemAt(int i)
	{
		return &this->m_Items[i];
	}
	ListItem* ListView::GetSelected()
	{
		int iPos = ListView_GetNextItem(this->handle, -1, LVNI_SELECTED);
		if(iPos != -1)
			return &this->m_Items[iPos];
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
		lvi.pszText = (LPWSTR)L"1";
		ListView_InsertItem(this->handle, (LPARAM)&lvi);
		UINT index = 0;
		for(std::wstring value : item->values)
		{
			lvi.iSubItem = index;
			lvi.pszText = (LPWSTR)value.c_str();
			ListView_SetItem(this->handle, (LPARAM)&lvi);
			index++;
		}
		this->m_Items.push_back(ListItem(*item));
		return GetLastError() == 0;
	}
}