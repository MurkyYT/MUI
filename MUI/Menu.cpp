#include "MUI.h"
namespace MUI
{
	void MenuBar::Add(Menu* menu)
	{
		this->m_menus.push_back(menu);
	}
	MenuBar::~MenuBar()
	{
		for (size_t i = 0; i < m_menus.size(); i++)
			delete m_menus[i];
	}
	Separator::Separator()
	{
		this->style = MF_SEPARATOR;
		this->type = UISeparator;
	}
	Menu::~Menu() 
	{
		for (size_t i = 0; i < m_separators.size(); i++)
			delete m_separators[i];
	}
	Menu::Menu(LPCWSTR text)
	{
		this->m_WindowName = text;
		this->type = UIMenu;
		this->style = MF_STRING;
	}
	void Menu::Add(Menu* menu)
	{
		this->m_childs.push_back(menu);
		this->m_menus.push_back(menu);
	}
	void Menu::Add(Separator* sep)
	{
		this->m_childs.push_back(sep);
		this->m_menus.push_back(NULL);
		this->m_separators.push_back(sep);
	}
	BOOL Menu::IsChecked()
	{
		MENUITEMINFOW itmInfo = {};
		ZeroMemory(&itmInfo, sizeof(MENUITEMINFOW));
		itmInfo.fMask = MIIM_STATE;
		itmInfo.cbSize = sizeof(itmInfo);
		GetMenuItemInfoW(this->m_ParentHMENU, this->id,FALSE, &itmInfo);
		DWORD error = GetLastError();
		if(!error)
			return itmInfo.fState & MF_CHECKED;
		throw std::exception("Couldn't get Menu item style");
	}
	void Menu::SetChecked(BOOL checked)
	{
		MENUITEMINFOW itmInfo = {};
		ZeroMemory(&itmInfo, sizeof(MENUITEMINFOW));
		itmInfo.fMask = MIIM_STATE;
		itmInfo.cbSize = sizeof(itmInfo);
		GetMenuItemInfoW(this->m_ParentHMENU, this->id, FALSE, &itmInfo);
		DWORD error = GetLastError();
		if (!error || !this->m_ParentHMENU)
		{
			if (checked) {
				itmInfo.fState = itmInfo.fState | MF_CHECKED;
				this->SetStyle(this->GetStyle() | MF_CHECKED);
			}
			else {
				itmInfo.fState = itmInfo.fState & ~MF_CHECKED;
				this->SetStyle(this->GetStyle() & ~MF_CHECKED);
			}
			SetMenuItemInfoW(this->m_ParentHMENU,this->id, FALSE, &itmInfo);
		}
	}
	void Menu::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (OnClick)
				OnClick(this,{uMsg,wParam,lParam});

		}
		}
		if (this->parent)
			S_HandleEvents(this->parent,uMsg, wParam,lParam);
	}
}