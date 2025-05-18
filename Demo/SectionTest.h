#pragma once
#include <MUI.h>

using namespace mui;
class SectionTest : public Window
{
public:
	SectionTest() { InitializeComponent(); }
private:
	void InitializeComponent()
	{
		SetTitle(L"Section Test");

		auto section = std::make_shared<Section>(L"Test Section");

		auto button = std::make_shared<Button>(L"test button");

		section->Children().Add(button);

		button = std::make_shared<Button>(L"test button");

		section->Children().Add(button);

		SetContent(section);
	}
};

