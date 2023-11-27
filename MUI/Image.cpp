#include "MUI.h"
namespace MUI
{
	Image::Image(int x, int y, int width, int height) 
	{
		this->m_hBmp = NULL;
		this->type = MUI::UIType::UIImage;
		this->m_ClassName = L"Static";
		this->m_WindowName = NULL;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_VISIBLE | WS_CHILD | SS_BITMAP;
	}
	void Image::SetImage(HBITMAP hBmp)
	{
		this->m_hBmp = hBmp;
		SendMessage(this->handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
}