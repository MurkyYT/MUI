#pragma once

#include "./UIElementCollection.h"

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
		const UIElementCollection& Children();
	private:
		StackLayoutOrientation m_orientation;
		UIElementCollection m_collection;
	};

}

