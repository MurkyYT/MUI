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
		this->style = WS_CHILD | WS_VISIBLE | ES_CENTER | WS_CLIPSIBLINGS;
		if (customColor)
			this->style |= BS_OWNERDRAW;
	}
	void Button::HandleEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			{
				if (this->OnClick)
					OnClick(this, { uMsg,wParam,lParam });

			}
			}
			if (this->parent)
				S_HandleEvents(this->parent, uMsg, wParam, lParam);
		}
		break;
		default:
			break;
		}
		
	}
}