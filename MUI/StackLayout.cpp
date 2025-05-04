#include "StackLayout.h"

mui::StackLayout::StackLayout(StackLayoutOrientation orientation)
{
	m_orientation = orientation;
}

const mui::UIElementCollection& mui::StackLayout::Children()
{
	return m_collection;
}