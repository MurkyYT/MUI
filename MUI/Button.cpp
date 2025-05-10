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

size_t mui::Button::GetMinHeight()
{
	return m_idealSize.cy;
}

size_t mui::Button::GetMinWidth()
{
	return m_idealSize.cx;
}

size_t mui::Button::GetMaxHeight()
{
	if (m_verticalAligment == Fill)
		return m_availableSize.bottom - m_availableSize.top;
	else
		return m_idealSize.cy;
}

size_t mui::Button::GetMaxWidth()
{
	if (m_horizontalAligment == Fill)
		return m_availableSize.right - m_availableSize.left;
	else
		return m_idealSize.cx;
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
	case WM_CTLCOLORSTATIC:
		return { TRUE , NULL };
	default:
		break;
	}

	return { FALSE,NULL };
}

BOOL mui::Button::SetText(const wchar_t* text)
{
	m_name = text;

	if (!m_hWnd || !m_parenthWnd)
		return TRUE;

	BOOL res = SetDlgItemText(m_parenthWnd, m_id, m_name.c_str());
	PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
	UpdateIdealSize();
	return res;
}