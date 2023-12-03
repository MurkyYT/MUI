#include "MUI.h"
namespace MUI {
	Button::Button(LPCWSTR text,BOOL customColor, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UIButton;
		this->m_ClassName = L"Button";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_CHILD | WS_VISIBLE | ES_CENTER;
		if (customColor)
			this->style |= BS_OWNERDRAW;
	}
	void Button::SubscribeToOnClick(void* func)
	{
		if (func)
			this->onEvent = func;
	}
}