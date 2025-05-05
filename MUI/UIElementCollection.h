#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "./UIElement.h"

namespace mui 
{

	class UIElementCollection
	{
	public:
		~UIElementCollection() { DeleteObject(m_hFont); }
		UIElementCollection();
		HFONT GetFontHandle() { return m_hFont; }
		void Add(std::shared_ptr<UIElement> element);
		void Remove(std::shared_ptr<UIElement> element);
		std::shared_ptr<UIElement>& ItemByID(DWORD id);
		BOOL IDExists(DWORD id);
		void Clear();
		void SetHWND(HWND hWnd) { m_parenthWnd = hWnd; };
		const std::vector<std::shared_ptr<UIElement>>& Items();
	private:
		std::vector<std::shared_ptr<UIElement>> m_items;
		std::unordered_map<DWORD, std::shared_ptr<UIElement>> m_indexToItem;
		HWND m_parenthWnd = NULL;
		HFONT m_hFont;
		DWORD m_index = 1;
		void RedrawWindow();
	};

}

