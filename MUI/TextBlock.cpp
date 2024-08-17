#include "MUI.h"
namespace MUI
{
	TextBlock::TextBlock(LPCWSTR text, int x, int y, int width, int height)
	{
		this->type = MUI::UIType::UITextBlock;
		this->m_ClassName = L"Static";
		this->m_WindowName = text;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->style = WS_CHILD | WS_VISIBLE | ES_CENTER | WS_CLIPSIBLINGS;
	}
	BOOL TextBlock::SetText(LPCWSTR text)
	{
		this->m_WindowName = text;
		return SetDlgItemText(this->windowHandle, this->id, this->m_WindowName);
	}
	LPCWSTR TextBlock::GetText() { return this->m_WindowName; }
}