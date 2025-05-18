#pragma once

#include "./UIElement.h"
#include "./Events.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace mui
{
	class TreeViewItem;
	class TreeView : public UIElement
	{
		friend class TreeViewItem;
	public:

		TreeView();

		void AddChild(const std::shared_ptr<TreeViewItem>& child);
		void RemoveChild(const std::shared_ptr<TreeViewItem>& child);
		void RemoveChild(TreeViewItem* child);

		DWORD GetIconIndex(HICON icon);

		size_t Count();

		TreeViewItem* GetSelectedItem();

		void SetTextColor(COLORREF color);
		void SetBackgroundColor(COLORREF color);
		void SetExpandButtonColor(COLORREF color);
		void SetSelectedColor(COLORREF color);

		EventCallback_t RightClick{ NULL };
		EventCallback_t DoubleClick{ NULL };
		EventCallback_t OnReturn{ NULL };
		EventCallback_t SelectionChanged{ NULL };
	private:
		void AddChilds(const std::vector<std::shared_ptr<mui::TreeViewItem>>& items, const std::shared_ptr<mui::TreeViewItem>& root);
		void SetHWND(HWND hWnd) override;
		void UpdateIdealSize() override;
		EventHandlerResult HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		DWORD m_iconIndex = 0;

		COLORREF m_textColor = RGB(0, 0, 0);
		COLORREF m_expandButtonColor = RGB(0, 0, 0);
		COLORREF m_selectedColor = RGB(217, 235, 249);
		COLORREF m_backgroundColor = RGB(255, 255, 255);

		std::unordered_map<HICON, DWORD> m_iconToIndex;
		std::vector<std::shared_ptr<TreeViewItem>> m_children;
		HIMAGELIST m_hImageList;
	};

	class TreeViewItem
	{
		friend class TreeView;
	public:
		TreeViewItem(const std::wstring& text, HICON icon)
		{
			m_text = text;
			m_hIcon = icon;
		}
		void AddChild(const std::shared_ptr<TreeViewItem>& child) 
		{
			child->m_parent = this; 
			m_children.push_back(child);
			if(m_treeView->GetHWND())
			{
				LockWindowUpdate(m_treeView->GetHWND());
				TVINSERTSTRUCT tvis = { 0 };
				tvis.hParent = m_hItem;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.lParam = (LPARAM)child.get();
				tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
				tvis.item.pszText = (LPWSTR)child->m_text.c_str();
				tvis.item.iImage = m_treeView->GetIconIndex(child->m_hIcon);
				tvis.item.iSelectedImage = m_treeView->GetIconIndex(child->m_hIcon);
				child->m_hItem = TreeView_InsertItem(m_treeView->GetHWND(), &tvis);
				child->m_treeView = m_treeView;
				m_treeView->UpdateIdealSize();
				PostMessage(m_treeView->m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
				LockWindowUpdate(NULL);
			}
		}
		void RemoveChild(const std::shared_ptr<TreeViewItem>& child)
		{
			auto it = std::find(m_children.begin(), m_children.end(), child);
			if (it != m_children.end())
			{
				if (m_treeView->GetHWND())
				{
					LockWindowUpdate(m_treeView->GetHWND());
					TreeView_DeleteItem(m_treeView->GetHWND(), child->m_hItem);
					m_treeView->UpdateIdealSize();
					PostMessage(m_treeView->m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
					LockWindowUpdate(NULL);
				}

				m_children.erase(it);
			}
		}
		void RemoveChild(TreeViewItem* child)
		{
			auto it = std::find_if(m_children.begin(), m_children.end(),
				[child](const std::shared_ptr<TreeViewItem>& ptr) {
					return ptr.get() == child;
				});

			if (it != m_children.end())
			{
				if (m_treeView->GetHWND())
				{
					LockWindowUpdate(m_treeView->GetHWND());
					TreeView_DeleteItem(m_treeView->GetHWND(), child->m_hItem);
					m_treeView->UpdateIdealSize();
					PostMessage(m_treeView->m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
					LockWindowUpdate(NULL);
				}

				m_children.erase(it);
			}
		}
		TreeViewItem* GetParent() { return m_parent; }
	private:
		std::vector<std::shared_ptr<TreeViewItem>> m_children;
		std::wstring m_text;
		HICON m_hIcon = NULL;
		HTREEITEM m_hItem = NULL;
		TreeViewItem* m_parent = NULL;
		TreeView* m_treeView = NULL;
	};
}