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

mui::UIElement::EventHandlerResult mui::Button::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
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
		return { TRUE , NULL };
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

	BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
	UpdateIdealSize();
	return res;
}