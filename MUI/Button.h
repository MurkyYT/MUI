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

		size_t GetMinWidth() override;
		size_t GetMinHeight() override;
		size_t GetMaxWidth() override;
		size_t GetMaxHeight() override;

		BOOL SetText(const wchar_t* text);

		EventCallback_t OnClick{ NULL };
	protected:
		void UpdateIdealSize() override;
	private:
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

}

