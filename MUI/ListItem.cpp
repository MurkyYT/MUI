#include "MUI.h"
namespace MUI
{
	ListItem::~ListItem()
	{
		values.clear();
	}
	ListItem::ListItem(int imageIndex, std::vector<std::wstring> values)
	{
		this->imageIndex = imageIndex;
		this->values = values;
	}
	void ListItem::SetImageIndex(int index) {
		this->imageIndex = imageIndex;
	}
	std::vector<std::wstring> ListItem::GetValues() { return this->values; }
	int ListItem::GetImageIndex() { return this->imageIndex; }
}