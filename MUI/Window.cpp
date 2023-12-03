#include "MUI.h"
typedef void (*func_type)(void);

namespace MUI 
{
	Window::Window(HINSTANCE hInstance) : m_hWnd(NULL), m_hInstance(hInstance) {}
	Window::~Window()
	{
		this->OnDestroy();
	}

	Window* Window::w_GetObject(HWND hWnd) {
		return (Window*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	void Window::SetBackroundColor(COLORREF color)
	{
		this->m_hBrushBackground = CreateSolidBrush(color);
	}
	void Window::Show(int cmdShow) {
		ShowWindow(this->m_hWnd, cmdShow);
		UpdateWindow(this->m_hWnd);
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
		this->v_RegisterClass(WINDOW_CLASS,iconId);

		this->m_hWnd = CreateWindowEx
		(
			0,
			WINDOW_CLASS,
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			NULL,
			NULL,
			this->m_hInstance,
			this
		);
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
		ShowWindow(this->m_hWnd, SW_SHOW);
	}
	void Window::Hide()
	{
		ShowWindow(this->m_hWnd, SW_HIDE);
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
		this->m_Assets[index] = comp;
		this->RepositionComponents();
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
			window->m_hWnd = hWnd;
		}
		else {
			window = w_GetObject(hWnd);
		}

		if (window) {
			switch (uMsg) {
#ifdef DEBUG
			case WM_PAINT:
			{
				PAINTSTRUCT  ps;
				HDC          hdc;
				hdc = BeginPaint(hWnd, &ps);
				Graphics graphics(hdc);
				Pen      pen(Gdiplus::Color(255, 0, 0, 255));
				RECT rect;
				if (GetClientRect(window->m_hWnd, &rect))
				{
					int width = rect.right - rect.left;
					int height = rect.bottom - rect.top;
					// draw grid rows and columns here, just a test right now
					int x = width / 4;
					for (size_t i = 1; i < 4; i++)
					{
						graphics.DrawLine(&pen, x * i, 0, x * i, height);
					}
				}
				EndPaint(hWnd, &ps);
			}
#endif
			case WM_COMMAND:
				window->OnCommand(wParam, lParam);
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
			case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize = window->MinSize;
				lpMMI->ptMaxTrackSize = window->MaxSize;
				break;
			}
			case WM_SIZE:
			{
				window->RepositionComponents();
				break;
			}
			case WM_CLOSE:
			{
				if (window->onClose)
					((func_type)window->onClose)();
#ifdef DEBUG
				GdiplusShutdown(window->gdiplusToken);
#endif // DEBUG
#if HIDE_ON_CLOSE
				window->Hide();
				return 0;
#endif // HIDE_ON_CLOSE
				break;

			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	void Window::RepositionComponents()
	{
		RECT rect;
		if (GetClientRect(this->m_hWnd, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			for (auto& comp : this->m_Assets)
				comp.second->reposition(height, width);
		}
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
	void Window::OnCommand(WPARAM wParam, LPARAM lParam) {
		(void)wParam;
		(void)lParam;

		if(this->m_Assets.find((UINT)wParam) != m_Assets.end())
		{
			UIComponent* comp = this->m_Assets[(UINT)wParam];
			if(comp->onEvent)
				((func_type)comp->onEvent)();
			if(comp->parent && comp->parent->onEvent)
				((func_type)comp->parent->onEvent)();
		}
	}
	void Window::OnDestroy() {
		DeleteObject(this->m_hFont);
		DeleteObject(this->m_hBrushBackground);
		this->m_Assets.erase(this->m_Assets.begin(), this->m_Assets.end());
		this->m_UnusedIndexes.erase(this->m_UnusedIndexes.begin(), this->m_UnusedIndexes.end());
		DeleteObject(&(this->m_Index));

		PostQuitMessage(0);
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