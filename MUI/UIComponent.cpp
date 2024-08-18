#include "MUI.h"

namespace MUI
{
	void UIComponent::reposition(int h,int w,int x,int y)
	{
		POINT pos
		{
			x = this->x,
			y = this->y
		};
		h = h - this->m_margin.bottom - this->m_margin.top;
		w = w - this->m_margin.left - this->m_margin.right;
		this->UpdateVerticalAligment(pos, h, y);
		this->UpdateHorizontalAligment(pos, w, x);
		pos.x = pos.x - this->m_margin.right + this->m_margin.left;
		pos.y = pos.y - this->m_margin.bottom + this->m_margin.top;
		MoveWindow(this->handle, pos.x, pos.y, min(w,this->width), min(h,this->height), TRUE);
		InvalidateRect(this->handle, NULL, FALSE);
		UpdateWindow(this->handle);
	}
	void UIComponent::UpdateVerticalAligment(POINT& pos, int h,int y)
	{
		switch (this->m_vAligment)
		{
		case MUI::Top:
			pos.y = y;
			break;
		case MUI::Bottom:
			pos.y = y + h - this->height;
			break;
		case MUI::Center:
			pos.y = y + h / 2 - this->height / 2;
			break;
		case MUI::Stretch:
			//pos.y = 0;
			this->height = h;
			break;
		}
	}
	void UIComponent::UpdateHorizontalAligment(POINT& pos, int w,int x)
	{
		switch (this->m_hAligment)
		{
		case MUI::Left:
			pos.x = x;
			break;
		case MUI::Right:
			pos.x = x + w - this->width;
			break;
		case MUI::Center:
			pos.x = x + w / 2 - this->width / 2;
			break;
		case MUI::Stretch:
			//pos.x = 0;
			this->width = w;
			break;
		}
	}
	void UIComponent::SetVerticalAligment(Aligment alg) 
	{
		if (alg == Left || alg == Right)
		{
			throw std::exception("Wrong aligment for 'vertical aligment'\n");
			return;
		}
		this->m_vAligment = alg;
	}
	void UIComponent::SetHorizontalAligment(Aligment alg)
	{
		if (alg == Top || alg == Bottom) {
			throw std::exception("Wrong aligment for 'horizontal aligment'\n");
			return;
		}
		this->m_hAligment = alg;
	}
	void UIComponent::SetMargin(Margin m) { this->m_margin = m; }
	BOOL UIComponent::SetStyle(DWORD newStyle)
	{
		this->style = newStyle;
		BOOL res = SetWindowLong(this->handle, GWL_STYLE, this->style) != NULL;
		SendMessage(this->handle, BM_SETSTYLE, newStyle, TRUE);
		SetWindowPos(this->handle, HWND_TOPMOST, this->x, this->y, this->width, this->height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		UpdateWindow(this->handle);
		return res;
	}
	DWORD UIComponent::GetStyle() {
		LONG style = GetWindowLong(this->handle, GWL_STYLE);
		return style ? style : this->style;
	}
	void UIComponent::Hide() { ShowWindow(this->handle, SW_HIDE); }
	void UIComponent::Show() { ShowWindow(this->handle, SW_SHOW); }
	BOOL UIComponent::IsHidden() { return !IsWindowVisible(this->handle); }
}