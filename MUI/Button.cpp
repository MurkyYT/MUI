#include "Button.h"

#include <CommCtrl.h>

mui::Button::Button(const wchar_t* text, int x, int y, int width, int height)
{
	m_name = text;
	m_class = L"Button";
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_style = WS_VISIBLE | ES_CENTER;
}

mui::Button::Button(const wchar_t* text, int x, int y) : Button(text,x,y,0,0)
{
}

mui::Button::Button(const wchar_t* text) : Button(text, 0, 0, 0, 0)
{
}

void mui::Button::UpdateIdealSize()
{
	SIZE size{};
	Button_GetIdealSize(m_hWnd, &size);
	m_idealSize = size;
}

void mui::Button::SetTextColor(COLORREF color)
{
	m_textColor = color;
}

void mui::Button::SetBackgroundColor(COLORREF color)
{
	DeleteObject(m_backroundBrush);
	m_backgroundColor = color;
	m_backroundBrush = CreateSolidBrush(m_backgroundColor);
}

void mui::Button::SetRegularColor(COLORREF color)
{
	m_regularColor = color;
}

void mui::Button::SetHoverColor(COLORREF color)
{
	m_hoverColor = color;
}

void mui::Button::SetBorderColor(COLORREF color)
{
	m_borderColor = color;
}

void mui::Button::SetPressedColor(COLORREF color)
{
	m_pressedColor = color;
}

mui::UIElement::EventHandlerResult mui::Button::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL isPressed = FALSE;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		isPressed = TRUE;
		break;
	case WM_LBUTTONUP:
		isPressed = FALSE;
		break;
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CUSTOMDRAW:
		{
			LPNMCUSTOMDRAW  customDrawItem = (LPNMCUSTOMDRAW)lParam;
			HPEN pen = CreatePen(PS_INSIDEFRAME, 0, m_enabled ? m_borderColor : RGB(233,233,233));
			HBRUSH selectbrush = m_enabled ? CreateSolidBrush(m_regularColor) : CreateSolidBrush(RGB(249,249,249));

			if (customDrawItem->uItemState & CDIS_HOT && m_enabled)
			{
				DeleteObject(selectbrush);
				selectbrush = CreateSolidBrush(isPressed ? m_pressedColor : m_hoverColor);
			}

			HGDIOBJ old_pen = SelectObject(customDrawItem->hdc, pen);
			HGDIOBJ old_brush = SelectObject(customDrawItem->hdc, selectbrush);

			RECT rc = customDrawItem->rc;
			rc.left += 1;
			rc.top += 1;
			rc.right -= 1;
			rc.bottom -= 1;
			SetBkMode(customDrawItem->hdc, TRANSPARENT);
			::SetTextColor(customDrawItem->hdc, m_enabled ? m_textColor : RGB(131,131,131));
			RoundRect(customDrawItem->hdc, customDrawItem->rc.left + 1, customDrawItem->rc.top + 1, customDrawItem->rc.right - 1, customDrawItem->rc.bottom - 1, 5, 5);
			DrawText(customDrawItem->hdc, m_name.c_str(), -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(customDrawItem->hdc, old_pen);
			SelectObject(customDrawItem->hdc, old_brush);
			DeleteObject(pen);
			DeleteObject(selectbrush);

			return { TRUE, CDRF_SKIPDEFAULT };
		}
		default:
			break;
		}
	}
	break;
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (this->OnClick) OnClick(this, { uMsg,wParam,lParam });
		}
		break;
		}
	}
	break;
	case WM_CTLCOLORBTN:
		return { TRUE , (LRESULT)m_backroundBrush };
	default:
		break;
	}

	return { FALSE,NULL };
}

BOOL mui::Button::SetText(const std::wstring& text)
{
	m_name = text;

	if (!m_hWnd || !m_parenthWnd)
		return TRUE;

	LockWindowUpdate(m_hWnd);
    BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
    InvalidateRect(m_hWnd, NULL, TRUE);
    LockWindowUpdate(NULL);
	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
	UpdateIdealSize();
	return res;
}