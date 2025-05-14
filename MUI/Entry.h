#pragma once

#include "./UIElement.h"
#include "./Events.h"

namespace mui
{
	class Entry : public UIElement
	{
	public:
		Entry(const wchar_t* text, int x, int y, int width, int height);
		Entry(const wchar_t* text, int x, int y);
		Entry(const wchar_t* text);

		BOOL SetText(const std::wstring& text);

		BOOL SetPlaceholder(const std::wstring& text);

		BOOL SetTextAligment(LayoutAligment aligment);

		BOOL SetMultiline(BOOL multiline);

		std::wstring GetText();

		void SetTextColor(COLORREF color);

		EventCallback_t TextChanged{ NULL };
		EventCallback_t Completed{ NULL };

	private:
		COLORREF m_color = RGB(0, 0, 0);
		std::wstring m_placeholder = L"";
		DWORD m_prevLineCount = 0;
		void SetHWND(HWND hWnd) override;
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
}