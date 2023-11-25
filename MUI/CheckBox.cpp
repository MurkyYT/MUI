#include "MUI.h"

namespace MUI {
	CheckBox::CheckBox(LPCWSTR text, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UICheckBox;
		this->m_ClassName = L"Button";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP;
	}
	void CheckBox::SubscribeToOnClick(void* func)
	{
		if (func)
			this->onClick = func;
	}
	BOOL CheckBox::IsChecked()
	{
		return IsDlgButtonChecked(this->windowHandle, this->id);
	}
	void CheckBox::SetChecked(BOOL checked)
	{
		CheckDlgButton(this->windowHandle, this->id, checked);
		if(this->onClick)
			((void (*)(void))(this->onClick))();
	}
}