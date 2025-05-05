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
BOOL  mui::UIElementCollection::IDExists(DWORD id)
{
	return m_indexToItem.find(id) != m_indexToItem.end();
}
std::shared_ptr<mui::UIElement>& mui::UIElementCollection::ItemByID(DWORD id)
{
	if(IDExists(id))
		return m_indexToItem[id];

	throw std::runtime_error("ID Not found");
}
void mui::UIElementCollection::Add(std::shared_ptr<UIElement> element)
{
	element->m_id = (DWORD)m_index;

	if (m_parenthWnd)
	{
		element->m_hWnd = CreateWindowEx(
			0,
			element->m_class,
			element->m_name,
			element->m_style | WS_CHILD,
			(int)element->m_x, (int)element->m_y, (int)element->m_width, (int)element->m_height,
			m_parenthWnd,
			(HMENU)element->m_id,
			GetModuleHandle(NULL),
			element.get()
		);
		SendMessage(
			element->m_hWnd,
			WM_SETFONT,
			(WPARAM)m_hFont,
			TRUE
		);

		element->m_parenthWnd = m_parenthWnd;

		if (element->m_subclass)
			SetWindowSubclass(element->m_hWnd, UIElement::CustomProc, (UINT_PTR)element.get(), NULL);

		ShowWindow(element->m_hWnd, SW_SHOW);
		RedrawWindow();
	}

	m_items.push_back(element);
	m_indexToItem[element->m_id] = element;
	m_index++;
}

void mui::UIElementCollection::RedrawWindow()
{
	HWND parent;
	HWND temp = m_parenthWnd;
	while (temp)
	{
		parent = temp;
		PostMessage(parent, WM_SIZE, NULL, NULL);
		temp = GetParent(parent);
	}
}

void mui::UIElementCollection::Remove(std::shared_ptr<UIElement> element)
{
	auto it = std::find(m_items.begin(), m_items.end(), element);

	if (it != m_items.end())
	{
		DestroyWindow(element->m_hWnd);
		m_items.erase(it);
		m_indexToItem.erase(element->m_id);
		RedrawWindow();
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

const std::vector<std::shared_ptr<mui::UIElement>>& mui::UIElementCollection::Items()
{
	return m_items;
}
