#include "MUI.h"
#include "DarkMode.h"
#include "Divider.h"
#include <memory>

namespace MUI
{
	int Window::m_Windows = 0;
	Window::Window() : m_hWnd(NULL), m_hInstance(GetModuleHandleW(NULL)) {
	}
	Window::~Window()
	{
		this->OnDestroy();
	}

	Window* Window::w_GetObject(HWND hWnd) {
		LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		return reinterpret_cast<Window*>(ptr);
	}

	void Window::SetBackroundColor(COLORREF color)
	{
		this->m_cBackground = color;
		this->m_hBrushBackground = CreateSolidBrush(color);
	}
	BOOL Window::Activate()
	{
		SetForegroundWindow(this->m_hWnd);
		if (GetForegroundWindow() != this->m_hWnd)
		{
			SwitchToThisWindow(this->m_hWnd, TRUE);
			Sleep(2);
			SetForegroundWindow(this->m_hWnd);
		}
		return GetForegroundWindow() == this->m_hWnd;
	}
	BOOL Window::Create(const wchar_t* title, int width, int height,DWORD iconId)
	{
		m_title = title;
		m_height = height;
		m_width = width;
		m_iconId = iconId;
#if USE_MULTIPLE_WINDOW_CLASSES
		std::wstring className = std::wstring(WINDOW_CLASS).append(std::to_wstring(m_Windows));
#else
		std::wstring className = std::wstring(WINDOW_CLASS);
#endif
		this->v_RegisterClass(className.c_str(), m_iconId);

		this->m_hWnd = CreateWindowEx
		(
			0,
			className.c_str(),
			m_title,
			WS_OVERLAPPEDWINDOW /*| WS_CLIPCHILDREN*/,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_width,
			m_height,
			NULL,
			NULL,
			this->m_hInstance,
			this
		);
		m_Windows++;
		m_Destroyed = FALSE;
#ifdef DEBUG
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif // DEBUG
		return m_hWnd != NULL;
	}
	BOOL Window::Create(const wchar_t* title, DWORD iconId)
	{
		return  this->Create(title, DEFAULT_SIZE, DEFAULT_SIZE, iconId);
	}
	void Window::Show()
	{
		if (m_Destroyed) {
			throw std::exception("Window was destroyed\n");
			return;
		}
		ShowWindow(this->m_hWnd, SW_SHOW);
	}
	void Window::Hide()
	{
		ShowWindow(this->m_hWnd, SW_HIDE);
	}
	void Window::HideAll()
	{
		for (size_t i = 1; i < m_Index; i++)
			m_Assets[i]->Hide();
	}
	void Window::ShowAll()
	{
		for (size_t i = 1; i < m_Index; i++)
			m_Assets[i]->Show();
	}
	void Window::SetGrid(Grid* grid)
	{
		SetWindowLongPtr(this->m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
		this->m_grid = std::shared_ptr<Grid>(grid);
		this->b_useGrid = TRUE;
		this->AddComponents(this->m_grid->GetComponents());
		std::vector<std::shared_ptr<Divider>> dividers = this->m_grid->GetDividers();

		Divider_Init(m_hInstance, DIVIDER_CLASS_NAME, CreateSolidBrush(MUI::Color::GRAY), DIVIDER_NOTIFY_MSG);

		m_grid->Reorder(m_hWnd,TRUE);

		for (std::shared_ptr<Divider> divider : dividers)
		{
		    if(divider->isVertical)
			{
				divider->y = m_grid->m_rows[divider->row]->y;
				divider->x = m_grid->m_columns[divider->column]->x;
				for (size_t i = 0; i < m_grid->m_rows[divider->row]->components.size(); i++) {
					UIComponent* comp = m_grid->m_rows[divider->row]->components[i];
					comp->m_margin.top += DIVIDER_SIZE;
				}
			    divider->width = this->GetSize().cx - divider->x;
				divider->height = DIVIDER_SIZE;
			}
			else
			{
				divider->x = m_grid->m_columns[divider->column]->x;
				divider->y = m_grid->m_rows[divider->row]->y;
				for (size_t i = 0; i < m_grid->m_columns[divider->column]->components.size(); i++) {
					UIComponent* comp = m_grid->m_columns[divider->column]->components[i];
					comp->m_margin.left += DIVIDER_SIZE;
				}
			    divider->height = this->GetSize().cy - divider->y;
				divider->width = DIVIDER_SIZE;
			}
			divider->hwnd = CreateWindowEx(0,DIVIDER_CLASS_NAME,L"",WS_CLIPSIBLINGS | WS_CHILD,divider->x,divider->y,divider->width,divider->height,this->m_hWnd,0,m_hInstance,NULL);
			ShowWindow(divider->hwnd, SW_SHOW);
		}
	}
	void Window::SetMenuBar(MenuBar* menu)
	{
		m_dockMenu.reset();
		this->m_dockMenu = std::shared_ptr<MenuBar>(menu);
		HMENU hMenubar = CreateMenu();
		for (size_t i = 0; i < menu->m_menus.size(); i++)
		{
			HMENU hMenu = CreateMenu();
			menu->m_menus[i]->m_hMenu = hMenu;
			AppendChilds(menu->m_menus[i], hMenu);
			menu->m_menus[i]->SetStyle(MF_POPUP);
			AppendMenuW(hMenubar, menu->m_menus[i]->style, (UINT_PTR)hMenu, menu->m_menus[i]->m_WindowName);
		}
		SetMenu(m_hWnd, hMenubar);
		DrawMenuBar(m_hWnd);
	}
	void Window::AppendChilds(Menu* menu,HMENU hMenu)
	{
		for (size_t i = 0; i < menu->m_childs.size(); i++)
		{
			uint64_t index = NULL;
			UIComponent* comp = menu->m_childs[i];
			if (comp->type != UISeparator) {
				index = this->m_Index;
				if (this->m_UnusedIndexes.size() != 0)
				{
					index = this->m_UnusedIndexes[0];
					this->m_UnusedIndexes.erase(this->m_UnusedIndexes.begin());
				}
				else
					this->m_Index++;
				m_Assets[index] = std::shared_ptr<UIComponent>(comp);
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
	void Window::AddComponents(std::vector<UIComponent*> comps)
	{
		for(UIComponent* comp : comps)
			this->AddComponent(comp);
	}
	BOOL Window::AddComponent(UIComponent* comp)
	{
		uint64_t index = this->m_Index;
		if (this->m_UnusedIndexes.size() != 0)
		{
			index = this->m_UnusedIndexes[0];
			this->m_UnusedIndexes.erase(this->m_UnusedIndexes.begin());
		}
		else
			this->m_Index++;
		comp->handle = CreateWindowEx(
			0,
			comp->m_ClassName,
			comp->m_WindowName,
			comp->style,
			comp->x, comp->y, comp->width, comp->height,
			this->m_hWnd,
			(HMENU)index,
			this->m_hInstance,
			0
		);
		SendMessage(
			comp->handle,
			WM_SETFONT,
			(WPARAM)this->m_hFont,
			TRUE
		);
		comp->id = (DWORD)index;
		comp->windowHandle = this->m_hWnd;
		this->m_Assets[index] = std::shared_ptr<UIComponent>(comp);

		comp->parentWindow = this;

		SetWindowSubclass(comp->handle, UIComponent::CustomProc, (UINT_PTR)comp, NULL);

		return TRUE;
	}
	void Window::v_RegisterClass(const wchar_t* name, DWORD iconId)
	{
		if(!this->m_hBrushBackground)
			this->m_hBrushBackground = CreateSolidBrush(MUI::Color::WHITE);

		WNDCLASSEX wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Window::WindowProc;
		wcex.hInstance = this->m_hInstance;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = this->m_hBrushBackground;
		wcex.hIcon = LoadIcon(this->m_hInstance, MAKEINTRESOURCE(iconId));
		wcex.lpszClassName = name;

		if (!RegisterClassEx(&wcex)) {
			return;
		}
	}
	LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Window* window = nullptr;

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			InitDarkMode();
			_AllowDarkModeForWindow(hWnd, TRUE);
			RefreshTitleBarThemeColor(hWnd);
			window->m_hWnd = hWnd;
		}
		else {
			window = w_GetObject(hWnd);
		}

		if (window) {
			switch (uMsg) {
			case WM_SETTINGCHANGE:
			{
				if(IsColorSchemeChangeMessage(lParam))
					RefreshTitleBarThemeColor(hWnd);
			}
			case WM_PAINT:
			{

				PAINTSTRUCT ps;
				RECT rect;
				HDC hdc = BeginPaint(window->m_hWnd, &ps);
				GetClientRect(window->m_hWnd, &rect);
				SetDCBrushColor(hdc, window->m_cBackground);
				FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
#ifdef DEBUG
				Graphics graphics(hdc);
				Pen      pen(Gdiplus::Color(255, 0, 0, 255));
				if (window->b_useGrid && window->m_grid && GetClientRect(window->m_hWnd, &rect))
				{
					int width = rect.right - rect.left;
					int height = rect.bottom - rect.top;
					for (std::shared_ptr<GridRow> row : window->m_grid->m_rows)
						graphics.DrawLine(&pen, 0, row->GetY(), width, row->GetY());
					for (std::shared_ptr<GridColumn> col : window->m_grid->m_columns)
						graphics.DrawLine(&pen, col->GetX(), 0, col->GetX(), height);
					graphics.DrawLine(&pen, width - 1, 0, width - 1, height);
					graphics.DrawLine(&pen, 0, height - 1, width - 1, height - 1);
				}
#endif
				EndPaint(window->m_hWnd, &ps);
			}

			case WM_COMMAND:
				window->OnCommand(uMsg,wParam, lParam);
				break;
			case WM_NOTIFY:
			{
				if (window->m_Assets.find(((LPNMHDR)lParam)->idFrom) != window->m_Assets.end())
				{
					UIComponent* comp = window->m_Assets[(UINT)wParam].get();
					comp->HandleEvents(uMsg, wParam, lParam);
				}
			}
			break;
			case WM_CREATE:
				window->OnCreate();
				return 0;
			case WM_DESTROY:
				window->OnDestroy();
				break;
			case WM_CTLCOLORSTATIC:
				return window->OnColorStatic(wParam);
			case WM_CTLCOLOREDIT:
				return window->OnColorEdit(wParam);
			case WM_CTLCOLORBTN:
				return window->OnColorButton(wParam);
			case WM_DRAWITEM:
				return window->OnDraw(wParam, lParam);
			case DIVIDER_NOTIFY_MSG:
			{
				switch (Divider_GetState(lParam))
				{
				case DIVIDER_START:
				{
					std::vector<std::shared_ptr<Divider>> dividers = window->m_grid->GetDividers();
					for (std::shared_ptr<Divider> div : dividers)
					{
						if (div->hwnd == (HWND)wParam)
						{
							div->strct = lParam;
							div->start = Divider_GetPosition(lParam);
							ScreenToClient(hWnd, &div->start);
						}
					}
				}
				break;
				case DIVIDER_MOVE:
				{
					std::vector<std::shared_ptr<Divider>> dividers = window->m_grid->GetDividers();
					for (std::shared_ptr<Divider> div : dividers)
					{
						if(div->hwnd == (HWND)wParam)
						{
							POINT pnt = Divider_GetPosition(lParam);
							ScreenToClient(hWnd, &pnt);
							if(div->isVertical)
							{
								if (pnt.y > window->m_grid->m_rows[div->row]->y + window->m_grid->m_rows[div->row]->height + DIVIDER_SIZE)
									div->y = window->m_grid->m_rows[div->row]->y + window->m_grid->m_rows[div->row]->height - DIVIDER_SIZE;
								else
									div->y = pnt.y;
							}
							else
							{
								if (pnt.x > window->m_grid->m_columns[div->column]->x + window->m_grid->m_columns[div->column]->width + DIVIDER_SIZE)
									div->x = window->m_grid->m_columns[div->column]->x + window->m_grid->m_columns[div->column]->width - DIVIDER_SIZE;
								else
									div->x = pnt.x;
							}
							break;
						}
					}
					if (window->m_grid) {
						window->m_grid->Reorder(window->m_hWnd);
						for (std::shared_ptr<GridItem> itm : window->m_grid->GetItems())
							window->m_grid->Reposition(itm.get());
					}
					break;
				}
				case DIVIDER_END:
					if (window->m_grid) {
						window->m_grid->Reorder(window->m_hWnd);
						for (std::shared_ptr<GridItem> itm : window->m_grid->GetItems())
							window->m_grid->Reposition(itm.get());
						for (std::shared_ptr<GridItem> itm : window->m_grid->GetItems())
						{
							InvalidateRect(itm->GetComponent()->handle, NULL, FALSE);
							UpdateWindow(itm->GetComponent()->handle);
						}
						
					}
					std::vector<std::shared_ptr<Divider>> dividers = window->m_grid->GetDividers();
					for (std::shared_ptr<Divider> div : dividers)
					{
						if (div->hwnd == (HWND)wParam)
						{
							div->start = POINT{ -1,-1 };
						}
					}
					break;
				}
				break;
			}
			case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize = window->MinSize;
				lpMMI->ptMaxTrackSize = window->MaxSize;
				break;
			}
			case WM_SIZE:
			{
				if (window->m_grid) {
					std::vector<std::shared_ptr<Divider>> dividers = window->m_grid->GetDividers();
					for (std::shared_ptr<Divider> divider : dividers)
					{
						if (divider->isVertical)
						{
							divider->width = window->GetSize().cx - divider->x;
							divider->height = DIVIDER_SIZE;
						}
						else
						{
							divider->height = window->GetSize().cy - divider->y;
							divider->width = DIVIDER_SIZE;
						}
					}
					window->m_grid->Reorder(window->m_hWnd,TRUE);
					for (std::shared_ptr<GridItem> itm : window->m_grid->GetItems())
						window->m_grid->Reposition(itm.get());
				}
				else
				{
					RECT rect;
					if (GetClientRect(window->m_hWnd, &rect))
					{
						for (size_t i = 1; i < window->m_Index; i++)
							window->m_Assets[i]->Reposition(
								rect.bottom - rect.top,
								rect.right - rect.left,0,0);
					}
				}
				
				break;
			}
			case WM_KEYDOWN:
			{
				if(window->OnKeyDown)
					window->OnKeyDown(window, { uMsg,wParam,lParam });
				break;
			}
			case WM_KEYUP:
			{
				if (window->OnKeyUp)
					window->OnKeyUp(window, { uMsg,wParam,lParam });
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT Window::OnColorButton(WPARAM wParam) {
		HDC hdcButton = (HDC)wParam;
		if (this->m_ButtonBacgkround) {
			HDC hdcButton = (HDC)wParam;
			if (this->m_ButtonTextColor)
				SetTextColor(hdcButton, this->m_ButtonTextColor);
			SetBkColor(hdcButton, this->m_ButtonBacgkround);
			return (LRESULT)CreateSolidBrush(this->m_ButtonBacgkround);
		}
		else
		{
			SetBkColor(hdcButton, Color::GRAY);
			return (LRESULT)CreateSolidBrush(Color::GRAY);
		}
		return TRUE;
	}

	LRESULT Window::OnColorEdit(WPARAM wParam) {
		HDC hdcEdit = (HDC)wParam;
		if (this->m_EditBacgkround) {

			if (this->m_EditTextColor)
				SetTextColor(hdcEdit, this->m_EditTextColor);
			SetBkColor(hdcEdit, this->m_EditBacgkround);
			return (LRESULT)CreateSolidBrush(this->m_EditBacgkround);
		}
		else
		{
			LOGBRUSH br = { 0 };
			GetObject(this->m_hBrushBackground, sizeof(br), &br);
			SetBkColor(hdcEdit, br.lbColor);
			return (LRESULT)this->m_hBrushBackground;
		}
		return TRUE;
	}
	LRESULT Window::OnColorStatic(WPARAM wParam) {
		HDC hdcStatic = (HDC)wParam;
		if (this->m_StaticBacgkround) {
			if(this->m_StaticTextColor)
				SetTextColor(hdcStatic, this->m_StaticTextColor);
			SetBkColor(hdcStatic, this->m_StaticBacgkround);
			return (LRESULT)CreateSolidBrush(this->m_StaticBacgkround);
		}
		else
		{
			LOGBRUSH br = { 0 };
			GetObject(this->m_hBrushBackground, sizeof(br), &br);
			SetBkColor(hdcStatic, br.lbColor);
			return (LRESULT)this->m_hBrushBackground;
		}
		return TRUE;
	}
	void Window::OnCommand(UINT uMsg,WPARAM wParam, LPARAM lParam) 
	{
		if(this->m_Assets.find((UINT)wParam) != m_Assets.end())
		{
			UIComponent* comp = this->m_Assets[(UINT)wParam].get();
			comp->HandleEvents(uMsg,wParam,lParam);
		}
	}
	void Window::OnDestroy() {
		if (!m_Destroyed) {
			DestroyWindow(m_hWnd);
			SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
			DeleteObject(this->m_hFont);
			DeleteObject(this->m_hBrushBackground);
			/*for (size_t i = 1; i < this->m_Index; i++)
				switch (m_Assets[i]->type)
				{
				case UISeparator:
				case UIMenu:
					break;
				default:
					delete this->m_Assets[i];
				}*/
			this->m_Assets.erase(this->m_Assets.begin(), this->m_Assets.end());
			this->m_UnusedIndexes.erase(this->m_UnusedIndexes.begin(), this->m_UnusedIndexes.end());
			m_dockMenu.reset();
			m_grid.reset();
			m_Index = 1;
			m_Windows--;
			m_Destroyed = TRUE;

			if (OnClose)
				OnClose(this, { });
#ifdef DEBUG
			GdiplusShutdown(gdiplusToken);
#endif // DEBUG

#if EXIT_ON_CLOSE
			if (m_Windows <= 0)
				PostQuitMessage(0);
#endif // EXIT_ON_CLOSE
		}
	}
	void Window::OnCreate()
	{
		NONCLIENTMETRICS ncm = {};
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

		this->m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
	}
	LRESULT Window::OnDraw(WPARAM wParam, LPARAM lParam) {
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
		DrawText(
			pdis->hDC,
			this->m_Assets[(UINT)wParam]->m_WindowName,
			lstrlen(this->m_Assets[(UINT)wParam]->m_WindowName),
			&pdis->rcItem,
			DT_CENTER | DT_SINGLELINE | DT_VCENTER
		);

		return TRUE;
	}
}

INT MUI::Window::GetWidth()
{
	RECT rect;
	if (GetWindowRect(m_hWnd, &rect))
		return rect.right - rect.left;

	return -1;
}

INT MUI::Window::GetHeight()
{
	RECT rect;
	if (GetWindowRect(m_hWnd, &rect))
		return rect.bottom - rect.top;;

	return -1;
}

void MUI::Window::SetHeight(INT height)
{
	SetWindowPos(m_hWnd, NULL, 0, 0, GetWidth(), height, SWP_NOMOVE);
}

void MUI::Window::SetWidth(INT width)
{
	SetWindowPos(m_hWnd, NULL, 0, 0, width , GetHeight(), SWP_NOMOVE);
}