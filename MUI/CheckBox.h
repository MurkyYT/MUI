#pragma once

#include "./UIElement.h"
#include "./Events.h"

namespace mui
{

	class CheckBox : public UIElement
	{
	public:
		CheckBox(const wchar_t* text, int x, int y, int width, int height);
		CheckBox(const wchar_t* text, int x, int y);
		CheckBox(const wchar_t* text);

		BOOL IsChecked();

		BOOL SetText(const std::wstring& text);

		void SetChecked(BOOL checked);

		void SetTextColor(COLORREF color);

		void SetBackgroundColor(COLORREF color);

		EventCallback_t StateChanged{ NULL };
	private:
		void SetParentHWND(HWND hWnd) override;
		SIZE GetAccurateCheckboxSize(HWND hWnd);
		BOOL m_checked = FALSE;
		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_backgroundColor = RGB(255,255,255);
		int m_checkBoxOffset = 0;
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}