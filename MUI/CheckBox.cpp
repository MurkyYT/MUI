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
		this->style = WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_CLIPSIBLINGS;
	}
	void CheckBox::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			if (this->parent)
				S_HandleEvents(this->parent, uMsg, wParam, lParam);

			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			{
				if (this->OnClick)
					OnClick(this, { uMsg,wParam,lParam });
			}
			}
		}
		break;
		default:
			break;
		}
	}
	BOOL CheckBox::IsChecked()
	{
		return IsDlgButtonChecked(this->windowHandle, this->id);
	}
	void CheckBox::SetChecked(BOOL checked)
	{
		CheckDlgButton(this->windowHandle, this->id, checked);
	}
}