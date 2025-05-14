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

		EventCallback_t StateChanged{ NULL };
	protected:
		void UpdateIdealSize() override;
	private:
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}