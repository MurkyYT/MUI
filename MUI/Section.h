#pragma once

#include "./UIElement.h"
#include "./StackLayout.h"
#include "./Events.h"

namespace mui 
{
	class Section : public UIElement
	{
	public:
		Section(const std::wstring& text);

		UIElementCollection& Children() { return m_stack.Children(); }

		void SetTextColor(COLORREF color);
		void SetBackgroundColor(COLORREF color);
		void SetExpandButtonColor(COLORREF color);

	private:
		StackLayout m_stack;
		std::wstring m_text;
		BOOL m_expanded = FALSE;

		void DrawTriangle(HDC hdc, RECT rc, bool expanded, COLORREF backgroundColor, COLORREF triangleColor);

		void UpdateIdealSize() override;
		void SetHWND(HWND hWnd) override;

		COLORREF m_backgroundColor = RGB(255, 255, 255);
		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_expandColor = RGB(0, 0, 0);

		SIZE m_textSize;

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
}