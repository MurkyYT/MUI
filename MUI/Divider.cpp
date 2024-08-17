#include "divider.h"
#include <tchar.h>

static ATOM g_aDivWndClass = NULL;
static DWORD g_dwDivNotifyMsg = NULL;
static HCURSOR g_hHorCur, g_hVertCur;
static TCHAR *g_szClassName = NULL;

static LRESULT CALLBACK DividerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL Divider_Init(HINSTANCE hInstance, LPCTSTR szClassName, HBRUSH hBackgroundBrush, DWORD dwNotifyMsg)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)DividerWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = hBackgroundBrush;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szClassName;
	wcex.hIconSm = NULL;

	g_szClassName = new TCHAR[_tcslen(szClassName)+1];
	_tccpy(g_szClassName, szClassName);

	g_aDivWndClass = RegisterClassEx(&wcex);

	g_dwDivNotifyMsg = dwNotifyMsg;

	g_hVertCur = LoadCursor(NULL, IDC_SIZEWE);
	g_hHorCur = LoadCursor(NULL, IDC_SIZENS);
	return g_aDivWndClass ? TRUE : FALSE;
}

BOOL Divider_Deinit(HINSTANCE hInstance)
{
	BOOL ret = FALSE;
	if (g_szClassName)
	{
		ret = UnregisterClass(g_szClassName, hInstance);
		delete[] g_szClassName;
		g_szClassName = NULL;
	}
	return ret;
}

POINT Divider_GetPosition(LPARAM ptr)
{
	sDividerMsgData* pData = (sDividerMsgData*)ptr;
	return pData->cur;
}

LONG Divider_GetMovement(LPARAM ptr)
{
	sDividerMsgData* pData = (sDividerMsgData*)ptr;
	return pData->bIsVeritcal ? pData->cur.x - pData->capt.x : pData->cur.y - pData->capt.y;
}
LONG Divider_GetState(LPARAM ptr)
{
	sDividerMsgData* pData = (sDividerMsgData*)ptr;
	return pData->state;
}

static LRESULT CALLBACK DividerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	sDividerMsgData *pData = NULL;
	RECT rct;
	HWND hParent;

	switch (message)
	{
	case WM_CREATE: {
		pData = new sDividerMsgData();
		LONG_PTR val = SetWindowLongPtr(hWnd, 0, (LONG_PTR)pData);
		return pData ? 0 : -1;
	}

	case WM_LBUTTONDOWN:
		pData = (sDividerMsgData*)GetWindowLongPtr(hWnd,0);
		if (!pData->bIsDragged)
		{
			SetCapture(hWnd);
			pData->bIsDragged = TRUE;
			pData->capt.x = (short)LOWORD(lParam);
			pData->capt.y = (short)HIWORD(lParam);

			ClientToScreen(hWnd,&(pData->capt));
			pData->prev = pData->cur = pData->capt;
			pData->state = DIVIDER_START;
			hParent = GetParent(hWnd);
			if (hParent)
				SendMessage(hParent, g_dwDivNotifyMsg,(WPARAM)hWnd,(LPARAM)pData);
		}
		return 0;

	case WM_MOUSEMOVE:
		pData = (sDividerMsgData*)GetWindowLongPtr(hWnd,0);
		GetClientRect(hWnd,&rct);
		pData->bIsVeritcal = rct.right < rct.bottom;
		SetCursor(pData->bIsVeritcal ? g_hVertCur : g_hHorCur);
		if (pData->bIsDragged)
		{
			POINT pt = {(short)LOWORD(lParam),(short)HIWORD(lParam)};
			ClientToScreen(hWnd,&pt);

			pData->prev = pData->cur;
			pData->cur = pt;
			hParent = GetParent(hWnd);
			pData->state = DIVIDER_MOVE;
			if (hParent)
				SendMessage(hParent, g_dwDivNotifyMsg,(WPARAM)hWnd,(LPARAM)pData);
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
	case WM_CAPTURECHANGED:
		pData = (sDividerMsgData*)GetWindowLongPtr(hWnd,0);
		pData->bIsDragged = FALSE;
		hParent = GetParent(hWnd);
		pData->state = DIVIDER_END;
		SendMessage(hParent, g_dwDivNotifyMsg,(WPARAM)hWnd,(LPARAM)pData);
		break;
	case WM_DESTROY:
		pData = (sDividerMsgData*)GetWindowLongPtr(hWnd,0);
		if (pData)
			delete pData;
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
