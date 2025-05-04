#include "UIElementCollection.h"

mui::UIElementCollection::UIElementCollection()
{

}

void mui::UIElementCollection::Add(UIElement* element)
{
	m_items.push_back(std::unique_ptr<UIElement>(element));
	InvalidateRect(element->m_parenthWnd, NULL, TRUE);
}

void mui::UIElementCollection::Remove(UIElement* element)
{
	auto it = std::find_if(m_items.begin(), m_items.end(),
		[element](const std::unique_ptr<UIElement>& ptr) {
			return ptr.get() == element;
		});

	if (it != m_items.end())
	{
		DestroyWindow(element->m_hWnd);
		m_items.erase(it);
		InvalidateRect(element->m_parenthWnd, NULL, TRUE);
	}
}

void mui::UIElementCollection::Clear()
{
	HWND parenthWnd = NULL;
	if(m_items.size() > 0)
		parenthWnd = m_items[0]->m_parenthWnd;

	m_items.clear();

	if(parenthWnd)
		InvalidateRect(parenthWnd, NULL, TRUE);
}

const std::vector<std::unique_ptr<mui::UIElement>>& mui::UIElementCollection::Items()
{
	return m_items;
}