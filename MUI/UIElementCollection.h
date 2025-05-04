#pragma once

#include <vector>
#include <memory>

#include "./UIElement.h"

namespace mui 
{

	class UIElementCollection
	{
	public:
		UIElementCollection();
		void Add(UIElement* element);
		void Remove(UIElement* element);
		void Clear();
		const std::vector<std::unique_ptr<UIElement>>& Items();
	private:
		std::vector<std::unique_ptr<UIElement>> m_items;
	};

}

