#include "MUI.h"
namespace MUI
{
	ContextMenu::~ContextMenu()
	{
		m_Assets.clear();
		m_childs.clear();
		m_menus.clear();

		DestroyMenu(m_hPopUp);
		DestroyWindow(handle);
	}
	void ContextMenu::Open()
	{
		POINT curPoint;
		GetCursorPos(&curPoint);

		TrackPopupMenu(
			m_hPopUp,
			TPM_LEFTBUTTON,
			curPoint.x,
			curPoint.y,
			0,
			handle,
			NULL
		);

	}
	ContextMenu::ContextMenu()
	{
		WNDCLASSEX wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = ContextMenu::SubclassWndProc;
		wcex.hInstance = GetModuleHandleW(NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"MUI_ContextMenu";

		if (!RegisterClassEx(&wcex)) {
			DWORD error = GetLastError();
			if (error != ERROR_CLASS_ALREADY_EXISTS)
				return;
		}

		handle = CreateWindowEx
		(
			0,
			L"MUI_ContextMenu",
			L"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			NULL,
			NULL,
			wcex.hInstance,
			this
		);
	}
	LRESULT CALLBACK ContextMenu::SubclassWndProc(HWND hwnd, UINT wm, WPARAM wParam, LPARAM lParam)
	{
		ContextMenu* menu;
		if (wm == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			menu = reinterpret_cast<ContextMenu*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(menu));
		}
		else
		{
			LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
			menu = (ContextMenu*)ptr;
		}
		if (menu) {
			switch (wm) {
			case WM_COMMAND:
			{
				if (menu->m_Assets.find((UINT)wParam) != menu->m_Assets.end())
				{
					UIComponent* comp = menu->m_Assets[(UINT)wParam];
					comp->HandleEvents(wm, wParam, lParam);
				}
			}
			break;
			case WM_CREATE:
			{
				menu->m_hPopUp = CreatePopupMenu();
				HMENU hMenuPop = menu->m_hPopUp;
			}
			break;
			}
		}
		return DefWindowProc(hwnd, wm, wParam, lParam);
	}
	void ContextMenu::AppendChilds(Menu* menu, HMENU hMenu)
	{
		for (size_t i = 0; i < menu->m_childs.size(); i++)
		{
			uint64_t index = this->m_Index;
			UIComponent* comp = menu->m_childs[i];
			this->m_Index++;
			if (comp->type != UISeparator) {
				m_Assets[index] = comp;
			}
			if (comp->type == UIMenu)
			{
				menu->m_menus[i]->m_ParentHMENU = hMenu;
				menu->m_menus[i]->id = (DWORD)index;
				if (menu->m_menus[i]->m_childs.size() > 0)
				{
					HMENU newHMenu = CreateMenu();
					menu->m_menus[i]->m_hMenu = newHMenu;
					menu->m_menus[i]->SetStyle(MF_POPUP);
					AppendChilds(menu->m_menus[i], newHMenu);
					AppendMenuW(hMenu, menu->m_menus[i]->style, (UINT_PTR)newHMenu, comp->m_WindowName);
				}
				else
					AppendMenuW(hMenu, comp->style, index, comp->m_WindowName);
			}
			else
			{
				AppendMenuW(hMenu, comp->style, index, comp->m_WindowName);
			}
		}
	}
	void ContextMenu::Add(Menu* menu)
	{
		if (menu->used)
			throw std::exception("Can't reuse same object more then once\n");
		this->m_childs.push_back(menu);
		this->m_menus.push_back(menu);
		uint64_t index = this->m_Index;
		UIComponent* comp = menu;
		this->m_Index++;
		this->m_Assets[index] = comp;
		menu->m_ParentHMENU = m_hPopUp;
		menu->id = (DWORD)index;
		menu->used = TRUE;
		if (menu->m_childs.size() > 0)
		{
			HMENU newHMenu = CreateMenu();
			menu->m_hMenu = newHMenu;
			menu->SetStyle(MF_POPUP);
			this->AppendChilds(menu, newHMenu);
			AppendMenuW(this->m_hPopUp, menu->style, (UINT_PTR)newHMenu, comp->m_WindowName);
		}
		else
			AppendMenuW(this->m_hPopUp, comp->style, index, comp->m_WindowName);
	}
	void ContextMenu::Add(Separator* menu)
	{
		this->m_menus.push_back(NULL);
		this->m_childs.push_back(menu);
		AppendMenuW(m_hPopUp, menu->style, this->m_Index, menu->m_WindowName);
		this->m_Index++;
	}
}