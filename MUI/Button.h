#pragma once

#include "./UIElement.h"
#include "./Events.h"

namespace mui
{

	class Button : public UIElement
	{
	public:
		Button(const wchar_t* text, int x, int y, int width, int height);
		Button(const wchar_t* text, int x, int y);
		Button(const wchar_t* text);

		BOOL SetText(const std::wstring& text);

		void SetTextColor(COLORREF color);
		void SetBackgroundColor(COLORREF color) override;
		void SetHoverColor(COLORREF color);
		void SetPressedColor(COLORREF color);
		void SetBorderColor(COLORREF color);

		EventCallback_t OnClick{ NULL };
	private:
		void UpdateIdealSize() override;
		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_borderColor = RGB(0, 120, 212);
		COLORREF m_hoverColor = RGB(224, 238, 249);
		COLORREF m_pressedColor = RGB(204, 228, 247);
		COLORREF m_regularColor = RGB(253, 253, 253);
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}

