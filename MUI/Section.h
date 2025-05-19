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

		void SetContent(const std::shared_ptr<UIElement>& content) { m_content = content; }

		void SetTextColor(COLORREF color);
		void SetBackgroundColor(COLORREF color) override;
		void SetExpandButtonColor(COLORREF color);

	private:
		std::shared_ptr<UIElement> m_content = NULL;
		std::wstring m_text;
		BOOL m_expanded = FALSE;

		void DrawTriangle(HDC hdc, RECT rc, bool expanded, COLORREF backgroundColor, COLORREF triangleColor);

		void UpdateIdealSize() override;
		void SetHWND(HWND hWnd) override;

		BOOL m_customBackground = FALSE;
		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_expandColor = RGB(0, 0, 0);

		SIZE m_textSize = { 0,0 };
		SIZE m_expandSize = { 0,0 };
		HFONT m_hFont = NULL;

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
}