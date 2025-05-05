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

		void SetHWND(HWND hWnd) override;

		UIElementCollection& Children();
	private:
		StackLayoutOrientation m_orientation;
		UIElementCollection m_collection;

		size_t CalcMinWidth();
		size_t CalcMaxWidth();
		size_t CalcMinHeight();
		size_t CalcMaxHeight();

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}

