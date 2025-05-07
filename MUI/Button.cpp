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
	SIZE size{};
	Button_GetIdealSize(m_hWnd, &size);
	//if (m_availableSize.bottom - m_availableSize.top < size.cy)
	//	return m_availableSize.bottom - m_availableSize.top;
	return size.cy;
}

size_t mui::Button::GetMinWidth()
{
	SIZE size{};
	Button_GetIdealSize(m_hWnd, &size);
	//if (m_availableSize.right - m_availableSize.left < size.cx)
	//	return m_availableSize.right - m_availableSize.left;
	return size.cx;
}

size_t mui::Button::GetMaxHeight()
{
	SIZE size{};
	Button_GetIdealSize(m_hWnd, &size);
	if (m_verticalAligment == Fill)
		return max(size.cy, m_availableSize.bottom - m_availableSize.top);
	else
		return size.cy;
}

size_t mui::Button::GetMaxWidth()
{
	SIZE size{};
	Button_GetIdealSize(m_hWnd, &size);
	if (m_horizontalAligment == Fill)
		return max(size.cx, m_availableSize.right - m_availableSize.left);
	else
		return size.cx;
}

void mui::Button::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		}
	}
	break;
	default:
		break;
	}
}