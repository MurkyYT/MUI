#pragma once

#include "./UIElementCollection.h"

#include <Windows.h>

namespace mui 
{
	enum StackLayoutOrientation
	{
		Vertical,
		Horizontal
	};

	class StackLayout : public UIElement
	{
	public:
		StackLayout(StackLayoutOrientation orientation);

		size_t GetMinWidth() override { return CalcMinWidth(); };
		size_t GetMinHeight() override { return CalcMinHeight(); };
		size_t GetMaxWidth() override { return CalcMaxWidth(); };
		size_t GetMaxHeight() override { return CalcMaxHeight(); };

		UIElementCollection& Children();

		void SetBackgroundColor(COLORREF color);
	private:
		BOOL m_insideAnotherStackLayout = FALSE;
		StackLayoutOrientation m_orientation;
		UIElementCollection m_collection;

		size_t CalcMinWidth();
		size_t CalcMaxWidth();
		size_t CalcMinHeight();
		size_t CalcMaxHeight();

		COLORREF m_backgroundColor = RGB(255, 255, 255);

		void SetHWND(HWND hWnd) override;
		void SetParentHWND(HWND p_hWnd) override;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}

