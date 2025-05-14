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

		EventCallback_t OnClick{ NULL };
	private:
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}

