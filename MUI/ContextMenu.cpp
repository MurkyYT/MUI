#include "MUI.h"
namespace MUI
{
	ContextMenu::~ContextMenu()
	{
		m_Assets.clear();
		m_childs.clear();
		m_menus.clear();
	}
	void ContextMenu::Open()
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
			if(error != ERROR_CLASS_ALREADY_EXISTS)
				return;
		}

		HWND dummyHWND = CreateWindowEx
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
				SetForegroundWindow(hwnd);
				HMENU hMenuPop = CreatePopupMenu();
				for (size_t i = 0; i < menu->m_childs.size(); i++)
				{
					HMENU hMenu = CreateMenu();
					uint64_t index = menu->m_Index;
					UIComponent* comp = menu->m_childs[i];
					if (comp->type != UISeparator) {
						menu->m_Index++;
						menu->m_Assets[index] = comp;
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
							menu->AppendChilds(menu->m_menus[i], newHMenu);
							AppendMenuW(hMenuPop, menu->m_menus[i]->style, (UINT_PTR)newHMenu, comp->m_WindowName);
						}
						else
							AppendMenuW(hMenuPop, comp->style, index, comp->m_WindowName);
					}
					else
					{
						AppendMenuW(hMenuPop, comp->style, index, comp->m_WindowName);
					}
				}

				// Get current mouse position.
				POINT curPoint;
				GetCursorPos(&curPoint);

				TrackPopupMenu(
					hMenuPop,
					TPM_LEFTBUTTON,
					curPoint.x,
					curPoint.y,
					0,
					hwnd,
					NULL
				);

				DestroyMenu(hMenuPop);
				delete menu;
				DestroyWindow(hwnd);
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
			if (comp->type != UISeparator) {
				this->m_Index++;
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
		this->m_childs.push_back(menu);
		this->m_menus.push_back(menu);
	}
	void ContextMenu::Add(Separator* menu)
	{
		this->m_menus.push_back(NULL);
		this->m_childs.push_back(menu);
	}
}