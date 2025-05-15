#pragma once

#include "./UIElement.h"

namespace mui 
{
	class Label : public UIElement
	{
	public:
		Label(const wchar_t* text, int x, int y, int width, int height);
		Label(const wchar_t* text, int x, int y);
		Label(const wchar_t* text);

		BOOL SetText(const std::wstring& text);

		BOOL SetTextAligment(LayoutAligment aligment);

		std::wstring GetText();

		void SetTextColor(COLORREF color);

		void SetBackgroundColor(COLORREF color);

	private:
		COLORREF m_textColor = RGB(0,0,0);
		COLORREF m_backgroundColor = RGB(255, 255, 255);
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
}