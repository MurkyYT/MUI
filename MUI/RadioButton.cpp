#include "MUI.h"
namespace MUI
{
	RadioButton::RadioButton(LPCWSTR text, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UIRadioButton;
		this->m_ClassName = L"Button";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_CLIPSIBLINGS;
	}
	void RadioButton::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (OnClick)
				OnClick(this,{uMsg,wParam,lParam});

		}
		}
		if (this->parent)
			S_HandleEvents(this->parent,uMsg, wParam,lParam);
	}
}