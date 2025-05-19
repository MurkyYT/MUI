#include "UIElementCollection.h"

#include <CommCtrl.h>
#include <stdexcept>

mui::UIElementCollection::UIElementCollection()
{
	NONCLIENTMETRICS ncm = {};
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	m_hFont = CreateFontIndirect(&ncm.lfMessageFont);
}
BOOL mui::UIElementCollection::IDExists(DWORD id)
{
	return m_indexToItem.find(id) != m_indexToItem.end();
}
std::shared_ptr<mui::UIElement>& mui::UIElementCollection::ItemByID(DWORD id)
{
	if(IDExists(id))
		return m_indexToItem[id];

	throw std::runtime_error("ID Not found");
}
void mui::UIElementCollection::Add(const std::shared_ptr<UIElement>& element)
{
	element->SetID((DWORD)m_index);

	if (m_parenthWnd)
	{
		LockWindowUpdate(m_parenthWnd);
		element->Initialize(m_parenthWnd, element->GetID(), m_hFont);
		LockWindowUpdate(NULL);

		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
	}

	m_items.push_back(element);
	m_indexToItem[element->GetID()] = element;
	m_index++;
}

void mui::UIElementCollection::Remove(const std::shared_ptr<UIElement>& element)
{
	auto it = std::find(m_items.begin(), m_items.end(), element);

	if (it != m_items.end())
	{
		LockWindowUpdate(m_parenthWnd);
		DestroyWindow(element->GetHWND());
		m_items.erase(it);
		m_indexToItem.erase(element->GetID());
		LockWindowUpdate(NULL);
		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
	}
}

void mui::UIElementCollection::Clear()
{
	for (const std::shared_ptr<UIElement>& element : m_items)
	{
		DestroyWindow(element->GetHWND());
		m_indexToItem.erase(element->GetID());
	}

	m_items.clear();

	if(m_parenthWnd)
		PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
}

const std::vector<std::shared_ptr<mui::UIElement>>& mui::UIElementCollection::Items()
{
	return m_items;
}
