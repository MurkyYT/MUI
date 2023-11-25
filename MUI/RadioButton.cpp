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
		this->style = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP;
	}
	void RadioButton::SubscribeToOnClick(void* func)
	{
		if (func)
			this->onClick = func;
	}
}