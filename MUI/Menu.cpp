#include "MUI.h"
namespace MUI
{
	void MenuBar::Add(Menu* menu)
	{
		if (menu->used)
			throw std::exception("Can't reuse same object more then once\n");
		menu->used = TRUE;
		this->m_menus.push_back(menu);
	}
	MenuBar::~MenuBar()
	{
		for (size_t i = 0; i < m_menus.size(); i++) {
			m_menus[i]->onlySeparatorsClear = TRUE;
			delete m_menus[i];
		}
	}
	Separator::Separator()
	{
		this->style = MF_SEPARATOR;
		this->type = UISeparator;
	}
	Menu::~Menu() 
	{
		Clear();
	}
	Menu::Menu(LPCWSTR text)
	{
		this->m_WindowName = text;
		this->type = UIMenu;
		this->style = MF_STRING;
	}
	void Menu::Clear()
	{
		for (size_t i = 0; i < m_separators.size(); i++)
			delete m_separators[i];
		if(!onlySeparatorsClear)
			for (size_t i = 0; i < m_childs.size(); i++)
				delete m_childs[i];
	}
	void Menu::Add(Menu* menu)
	{
		if (menu->used)
			throw std::exception("Can't reuse same object more then once\n");
		menu->used = TRUE;
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
		if (!checked) {
			this->SetStyle(this->GetStyle() & ~MF_CHECKED);
		}
		else {
			this->SetStyle(this->GetStyle() | MF_CHECKED);
		}
		ModifyMenu(this->m_ParentHMENU, this->id, MF_BYCOMMAND | this->GetStyle(), this->id, this->m_WindowName);
	}
	void Menu::SetEnabled(BOOL enabled)
	{
		if (enabled) {
			this->SetStyle(this->GetStyle() & ~MF_DISABLED);
		}
		else {
			this->SetStyle(this->GetStyle() | MF_DISABLED);
		}
		ModifyMenu(this->m_ParentHMENU, this->id, MF_BYCOMMAND | this->GetStyle(), this->id, this->m_WindowName);
	}
	void Menu::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			if (this->parent)
				S_HandleEvents(this->parent, uMsg, wParam, lParam);

			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			{
				if (OnClick)
					OnClick(this, { uMsg,wParam,lParam });

			}
			}
		}
		break;
		}
	}
}