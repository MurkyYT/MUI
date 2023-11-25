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
		this->style = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL;
		if (number)
			this->style |= ES_NUMBER;
		this->isNum = number;
	}
	std::wstring TextBox::GetText()
	{
		wchar_t buffer[MAX_PATH];
		int suc = GetDlgItemText(
			this->windowHandle,
			this->id,
			buffer,
			MAX_PATH
		);
		if (suc)
			return std::wstring(buffer);
		return std::wstring(L"");
	}
	int TextBox::GetNumber()
	{
		BOOL suc = FALSE;
		int res = GetDlgItemInt(this->windowHandle, this->id, &suc,TRUE);
		if (suc)
			return res;
		return NULL;
	}
}