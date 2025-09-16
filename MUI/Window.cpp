#include "Window.h"
#include "Menu.h"

#include <dwmapi.h>
#include <CommCtrl.h>
#include <string>
#include <stdexcept>
#include <Rpc.h>

#pragma region UAHMenuBar
#include <vsstyle.h>

// https://stackoverflow.com/a/77998158

// window messages related to menu bar drawing
#define WM_UAHDESTROYWINDOW    0x0090   // handled by DefWindowProc
#define WM_UAHDRAWMENU         0x0091   // lParam is UAHMENU
#define WM_UAHDRAWMENUITEM     0x0092   // lParam is UAHDRAWMENUITEM
#define WM_UAHINITMENU         0x0093   // handled by DefWindowProc
#define WM_UAHMEASUREMENUITEM  0x0094   // lParam is UAHMEASUREMENUITEM
#define WM_UAHNCPAINTMENUPOPUP 0x0095   // handled by DefWindowProc

// describes the sizes of the menu bar or menu item
typedef union tagUAHMENUITEMMETRICS
{
	// cx appears to be 14 / 0xE less than rcItem's width!
	// cy 0x14 seems stable, i wonder if it is 4 less than rcItem's height which is always 24 atm
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizeBar[2];
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizePopup[4];
} UAHMENUITEMMETRICS;

// not really used in our case but part of the other structures
typedef struct tagUAHMENUPOPUPMETRICS
{
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2; // from kernel symbols, padded to full dword
} UAHMENUPOPUPMETRICS;

// hmenu is the main window menu; hdc is the context to draw in
typedef struct tagUAHMENU
{
	HMENU hmenu;
	HDC hdc;
	DWORD dwFlags; // no idea what these mean, in my testing it's either 0x00000a00 or sometimes 0x00000a10
} UAHMENU;

// menu items are always referred to by iPosition here
typedef struct tagUAHMENUITEM
{
	int iPosition; // 0-based position of menu item in menubar
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
} UAHMENUITEM;

// the DRAWITEMSTRUCT contains the states of the menu items, as well as
// the position index of the item in the menu, which is duplicated in
// the UAHMENUITEM's iPosition as well
typedef struct UAHDRAWMENUITEM
{
	DRAWITEMSTRUCT dis; // itemID looks uninitialized
	UAHMENU um;
	UAHMENUITEM umi;
} UAHDRAWMENUITEM;

// the MEASUREITEMSTRUCT is intended to be filled with the size of the item
// height appears to be ignored, but width can be modified
typedef struct tagUAHMEASUREMENUITEM
{
	MEASUREITEMSTRUCT mis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHMEASUREMENUITEM;

static void UAHDrawMenuNCBottomLine(HWND hWnd, mui::Menu* menu)
{
	HBRUSH brBarBackground = CreateSolidBrush(menu->GetBottomBarColor());
	MENUBARINFO mbi = { sizeof(mbi) };
	if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi))
	{
		return;
	}
	RECT rcClient = { 0 };
	GetClientRect(hWnd, &rcClient);
	MapWindowPoints(hWnd, nullptr, (POINT*)&rcClient, 2);

	RECT rcWindow = { 0 };
	GetWindowRect(hWnd, &rcWindow);
	OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

	RECT rcAnnoyingLine = rcClient;
	rcAnnoyingLine.bottom = rcAnnoyingLine.top;
	rcAnnoyingLine.top--;

	HDC hdc = GetWindowDC(hWnd);
	FillRect(hdc, &rcAnnoyingLine, brBarBackground);
	DeleteObject(brBarBackground);
	ReleaseDC(hWnd, hdc);
}

// processes messages related to UAH / custom menubar drawing.
// return true if handled, false to continue with normal processing in your wndproc
static bool UAHWndProc(mui::Menu* menu, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
{
	if (menu) {
		switch (message)
		{
		case WM_UAHDRAWMENU:
		{
			HBRUSH brBarBackground = CreateSolidBrush(menu->GetBackgroundColor());
			UAHMENU* pUDM = (UAHMENU*)lParam;
			RECT rc = { 0 };
			{
				MENUBARINFO mbi = { sizeof(mbi) };
				GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

				RECT rcWindow;
				GetWindowRect(hWnd, &rcWindow);
				rc = mbi.rcBar;
				OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
			}
			FillRect(pUDM->hdc, &rc, brBarBackground);

			DeleteObject(brBarBackground);
			return true;
		}
		case WM_UAHDRAWMENUITEM:
		{
			UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;

			HBRUSH brItemBackground = CreateSolidBrush(menu->GetItemBackgroundColor());
			HBRUSH brItemBackgroundHot = CreateSolidBrush(menu->GetItemBackgroundHotColor());
			HBRUSH brItemBackgroundSelected = CreateSolidBrush(menu->GetItemBackgroundSelectedColor());

			HBRUSH* pbrBackground = &brItemBackground;
			wchar_t menuString[256] = { 0 };
			MENUITEMINFO mii = { sizeof(mii), MIIM_STRING };
			{
				mii.dwTypeData = menuString;
				mii.cch = (sizeof(menuString) / 2) - 1;

				GetMenuItemInfo(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
			}

			DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
			int iTextStateID = 0;
			int iBackgroundStateID = 0;
			{
				if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT)) {
					iTextStateID = MPI_NORMAL;
					iBackgroundStateID = MPI_NORMAL;
				}
				if (pUDMI->dis.itemState & ODS_HOTLIGHT) {
					iTextStateID = MPI_HOT;
					iBackgroundStateID = MPI_HOT;
					pbrBackground = &brItemBackgroundHot;
				}
				if (pUDMI->dis.itemState & ODS_SELECTED) {
					iTextStateID = MPI_HOT;
					iBackgroundStateID = MPI_HOT;
					pbrBackground = &brItemBackgroundSelected;
				}
				if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED)) {
					iTextStateID = MPI_DISABLED;
					iBackgroundStateID = MPI_DISABLED;
				}
				if (pUDMI->dis.itemState & ODS_NOACCEL) {
					dwFlags |= DT_HIDEPREFIX;
				}
			}

			DTTOPTS opts = { sizeof(opts), DTT_TEXTCOLOR, iTextStateID != MPI_DISABLED ? RGB(0x00, 0x00, 0x20) : RGB(0x40, 0x40, 0x40) };
			FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);
			SetBkMode(pUDMI->um.hdc, TRANSPARENT);
			SetTextColor(pUDMI->um.hdc, menu->GetTextColor());
			DrawText(pUDMI->um.hdc, menuString, mii.cch, &pUDMI->dis.rcItem, dwFlags);
			return true;
		}
		case WM_UAHMEASUREMENUITEM:
		{
			UAHMEASUREMENUITEM* pMmi = (UAHMEASUREMENUITEM*)lParam;

			*lr = DefWindowProc(hWnd, message, wParam, lParam);
			return true;
		}
		case WM_NCPAINT:
		case WM_NCACTIVATE:
			*lr = DefWindowProc(hWnd, message, wParam, lParam);
			UAHDrawMenuNCBottomLine(hWnd, menu);
			return true;
			break;
		default:
			return false;
		}
	}
	return false;
}
#pragma endregion

#pragma comment(lib, "dwmapi")
#pragma comment (lib, "comctl32")
#pragma comment (lib, "rpcrt4")

BOOL mui::Window::s_dpiAware = SetProcessDPIAware();

mui::Window::~Window()
{
	DeleteObject(m_hFont);
	DestroyIcon(m_hIcon);
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
	if (m_content)
	{
		DestroyWindow(m_content->GetHWND());
		SetWindowLongPtr(m_content->GetHWND(), GWLP_USERDATA, NULL);
	}
	m_content = NULL;
	EventArgs_t args = { 0, 0,0, FALSE };
	if (OnClose)
		OnClose(this, &args);
}

mui::Window::Window(const std::wstring& title, size_t height, size_t width)
{
	if (!s_dpiAware)
		s_dpiAware = SetProcessDPIAware();

	UUID uuid;
	RPC_STATUS status = UuidCreate(&uuid);

	if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY)
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(status);
		throw std::runtime_error(err);
		return;
	}

	RPC_WSTR strUuid = NULL;
	status = UuidToString(&uuid, &strUuid);

	if (status != RPC_S_OK)
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(status);
		throw std::runtime_error(err);
		return;
	}

	std::wstring className = L"MUI_Window [" + std::wstring((const wchar_t*)strUuid) + L"]";

	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);

	ExtractIconExW(buffer, 0, &m_hIcon, NULL, 1);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszClassName = className.c_str();
	wcex.hIcon = m_hIcon;

	if (!RegisterClassEx(&wcex))
	{
		std::string err = std::string("Class creation failed: ") + std::to_string(GetLastError());
		throw std::runtime_error(err);
		return;
	}

	m_hWnd = CreateWindowEx
	(
		0,
		className.c_str(),
		title.c_str(),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(int)width,
		(int)height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this
	);

	if (!m_hWnd)
	{
		DWORD err = GetLastError();
		throw std::runtime_error("Window creation failed (" + std::to_string(err) + ")");
		return;
	}

	DragAcceptFiles(m_hWnd, TRUE);
}

void mui::Window::SetMenu(std::shared_ptr<Menu> menu)
{
	if (m_menu)
		m_menu->DetachFromWindow(m_hWnd);

	m_menu = menu;

	if (m_menu)
		m_menu->AttachToWindow(m_hWnd);
}
void mui::Window::SetCustomIcon(HICON icon)
{
	DestroyIcon(m_hIcon);
	m_hIcon = CopyIcon(icon);

	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
}

void mui::Window::Activate()
{
	if (IsIconic(m_hWnd))
		SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);

	SetForegroundWindow(m_hWnd);
	SetActiveWindow(m_hWnd);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	RedrawWindow(m_hWnd, NULL, 0, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

BOOL mui::Window::SetCaptionColor(COLORREF color)
{
	LRESULT result = DwmSetWindowAttribute(m_hWnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
	if (result < 0)
	{
		LPWSTR messageBuffer = nullptr;

		DWORD size = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			(DWORD)result,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&messageBuffer,
			0,
			NULL
		);

		std::wstring message = std::wstring(L"Set Caption Color failed: ") + ((size && messageBuffer) ? messageBuffer : L"Unknown error") + L"\n";

		if (messageBuffer)
			LocalFree(messageBuffer);
		
		OutputDebugString(message.c_str());

		return FALSE;
	}

	m_captionColor = color;

	return TRUE;
}

void mui::Window::Show()
{
	ShowWindow(m_hWnd, SW_SHOW);
}

size_t mui::Window::GetHeight()
{
	RECT rect = GetRect();
	return rect.bottom - rect.top;
}

size_t mui::Window::GetWidth() 
{
	RECT rect = GetRect();
	return rect.right - rect.left;
}

POINT mui::Window::GetTopLeft()
{
	RECT rect = GetRect();
	return { rect.left, rect.top };
}

RECT mui::Window::GetRect()
{
	RECT rect{};
	GetWindowRect(m_hWnd, &rect);
	return rect;
}

void mui::Window::SetContent(const std::shared_ptr<UIElement>& element)
{
	if (m_content) 
		DestroyWindow(m_content->GetHWND());

	m_content = element;

	element->Initialize(m_hWnd, (DWORD)1, m_hFont);
}

BOOL mui::Window::SetTitle(const std::wstring& title)
{
	return SetWindowText(m_hWnd, title.c_str());
}

std::wstring mui::Window::GetTitle()
{
	std::wstring text;
	text.resize(GetWindowTextLengthW(m_hWnd) + 1);
	text.resize(GetWindowTextW(m_hWnd, (LPWSTR)text.data(), (int)text.size() + 1));
	return text;
}

void mui::Window::Close()
{
	DestroyWindow(m_hWnd);
}

void mui::Window::SetWidth(size_t width)
{
	SetWindowPos(m_hWnd, NULL, 0, 0, (int)width, (int)GetHeight(), SWP_NOMOVE);
}

void mui::Window::SetHeight(size_t height)
{
	SetWindowPos(m_hWnd, NULL, 0, 0, (int)GetWidth(), (int)height, SWP_NOMOVE);
}

void mui::Window::SetMaxWidth(size_t width)
{
	m_maxSize.x = (LONG)width;
}

void mui::Window::SetMaxHeight(size_t height)
{
	m_maxSize.y = (LONG)height;
}

void mui::Window::SetMinWidth(size_t width)
{
	m_minSize.x = (LONG)width;
}

void mui::Window::SetMinHeight(size_t height)
{
	m_minSize.y = (LONG)height;
}

LRESULT CALLBACK mui::Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	Window* window;
	if (uMsg == WM_NCCREATE) 
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->m_hWnd = hWnd;
	}
	else 
		window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (window) 
	{
		LRESULT lr = 0;
		if (window->m_menu && UAHWndProc(window->m_menu.get(), hWnd, uMsg, wParam, lParam, &lr)) {
			return lr;
		}

		switch (uMsg)
		{
		case WM_DROPFILES:
		{
			EventArgs_t args = { uMsg, wParam, lParam };

			if (window->DragAndDrop)
				window->DragAndDrop(window, &args);

			DragFinish((HDROP)wParam);
			return 0;
		}
		break;
		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			RECT rc;
			GetClientRect(hWnd, &rc);
			HBRUSH hBrush = CreateSolidBrush(window->m_backgroundColor);
			FillRect(hdc, &rc, hBrush);

			DeleteObject(hBrush);
			return 1;
		}
		case WM_DESTROY:
		case WM_CLOSE:
		{
			DeleteObject(window->m_hFont);
			DestroyIcon(window->m_hIcon);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			if (window->m_content) 
			{
				DestroyWindow(window->m_content->GetHWND());
				SetWindowLongPtr(window->m_content->GetHWND(), GWLP_USERDATA, NULL);
			}
			window->m_content = NULL;
			EventArgs_t args = { uMsg, wParam,lParam, FALSE };
			if (window->OnClose)
				window->OnClose(window, &args);
		}
		break;
		case MUI_WM_REDRAW:
			// Intentional fall through
		case WM_SIZE:
		{
			if(window->m_content)
			{
				RECT rect{};
				GetClientRect(hWnd, &rect);
				window->m_content->SetAvailableSize(rect);
				SetWindowPos(window->m_content->GetHWND(), NULL,
					(int)window->m_content->GetX(),
					(int)window->m_content->GetY(),
					(int)window->m_content->GetMaxWidth(),
					(int)window->m_content->GetMaxHeight() , 
					NULL);
				InvalidateRect(window->m_content->GetHWND(), NULL, TRUE);
			}
		}
		break;
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize = window->m_minSize;
			lpMMI->ptMaxTrackSize = window->m_maxSize;
			break;
		}
		case WM_COMMAND:
		{
			if (window->m_content && LOWORD(wParam) == window->m_content->GetID())
			{
				UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
			else if(window->m_menu)
				window->m_menu->HandleMenuCommand(LOWORD(wParam));
		}
		break;
		case WM_NOTIFY:
		{
			if (window->m_content && ((LPNMHDR)lParam)->idFrom == window->m_content->GetID())
			{
				UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			HWND hwnd = (HWND)lParam;
			if (hwnd != NULL) {
				int controlId = GetDlgCtrlID(hwnd);
				if (window->m_content && controlId == window->m_content->GetID())
				{
					mui::UIElement::EventHandlerResult res = window->m_content->HandleEvent(uMsg, wParam, lParam);
					if (res.returnVal)
						return res.value;
				}
			}
		}
		break;
		case WM_CREATE:
		{
			NONCLIENTMETRICS ncm = {};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			window->m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
		}
		break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			EventArgs_t args = { uMsg, wParam,lParam, FALSE };
			if (window->KeyDown)
				window->KeyDown(window, &args);
		}
		break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			EventArgs_t args = { uMsg, wParam,lParam, FALSE };
			if (window->KeyUp)
				window->KeyUp(window, &args);
		}
		break;
		default:
			break;
		}

		EventArgs_t args = { uMsg, wParam, lParam };
		if (window->WndProc)
			window->WndProc(window, &args);

		if (args.handled)
			return args.msg;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}