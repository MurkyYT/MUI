#include "MUI.h"
namespace MUI
{
	RadioGroup::RadioGroup(LPCWSTR text, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UIRadioGroup;
		this->m_ClassName = L"Button";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_CENTER;
	}
	void RadioGroup::AddRadioButton(RadioButton* button)
	{
		this->m_Buttons.push_back(button);
		button->parent = this;
	}
	void RadioGroup::SubscribeToOnChange(void* func)
	{
		if (func)
			this->onClick = func;
	}
	int RadioGroup::CurrentRadioButton()
	{
		for (size_t i = 0; i < this->m_Buttons.size(); i++)
		{
			if (IsDlgButtonChecked(this->windowHandle, this->m_Buttons[i]->id))
				return i;
		}
		return -1;
	}
}