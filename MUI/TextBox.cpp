#include "MUI.h"

namespace MUI {
	TextBox::TextBox(LPCWSTR text,BOOL number, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UITextBox;
		this->m_ClassName = L"Edit";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | WS_CLIPSIBLINGS;
		if (number)
			this->style |= ES_NUMBER;
		this->isNum = number;
	}
	std::wstring TextBox::GetText()
	{
		std::wstring res;
		int len = GetWindowTextLengthW(this->handle) + 1;
		wchar_t* buffer = new wchar_t[len];
		int suc = GetDlgItemText(
			this->windowHandle,
			this->id,
			buffer,
			len
		);
		if (suc)
			res = std::wstring(buffer);
		else 
			res = std::wstring(L"");
		delete[] buffer;
		return res;
	}
	int TextBox::GetNumber()
	{
		BOOL suc = FALSE;
		int res = GetDlgItemInt(this->windowHandle, this->id, &suc,TRUE);
		if (suc)
			return res;
		return NULL;
	}
	/*void TextBox::EnableHorizontalScrollbar(BOOL isEnabled)
	{
		if(isEnabled)
			this->SetStyle(this->GetStyle() | WS_HSCROLL);
		else
			this->SetStyle(this->GetStyle() & ~WS_HSCROLL);
		BOOL suc = ShowScrollBar(this->handle, SB_HORZ, isEnabled);
		SetWindowPos(this->handle,this->handle,this->x,this->y,this->x,this->y, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);
		suc = InvalidateRect(this->handle,NULL,NULL);
	}
	void TextBox::EnableVerticalScrollbar(BOOL isEnabled)
	{
		if (isEnabled)
			this->SetStyle(this->GetStyle() | WS_VSCROLL);
		else
			this->SetStyle(this->GetStyle() & ~WS_VSCROLL);
		BOOL suc = ShowScrollBar(this->handle, SB_VERT, isEnabled);
		suc = InvalidateRect(this->handle, NULL, NULL);
	}
	BOOL TextBox::HorizontalScrollbarEnabled()
	{
		return this->GetStyle() & WS_HSCROLL;
	}
	BOOL TextBox::VerticalScrollbarEnabled()
	{
		return this->GetStyle() & WS_VSCROLL;
	}*/
}