#include "TreeView.h"

void MeasureItem(HWND hwndTV, HDC hdc, HTREEITEM hItem, int depth,
    int indentPerLevel, int expandCollapseButtonWidth,
    int iconWidth, int padding,
    SIZE* pMaxSize, int* pTotalItems)
{
    if (!hItem)
        return;

    TCHAR text[512];
    TVITEM tvi = { 0 };
    tvi.mask = TVIF_HANDLE | TVIF_TEXT;
    tvi.hItem = hItem;
    tvi.pszText = text;
    tvi.cchTextMax = sizeof(text) / sizeof(text[0]);

    if (TreeView_GetItem(hwndTV, &tvi))
    {
        SIZE size = { 0 };
        if (GetTextExtentPoint32(hdc, text, lstrlen(text), &size))
        {
            int totalWidthForItem =
                (depth * indentPerLevel) +
                expandCollapseButtonWidth +
                iconWidth +
                padding +
                size.cx;

            if (totalWidthForItem > pMaxSize->cx)
                pMaxSize->cx = totalWidthForItem;
        }
        (*pTotalItems)++;
    }

    HTREEITEM hChild = (HTREEITEM)SendMessage(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
    while (hChild)
    {
        MeasureItem(hwndTV, hdc, hChild, depth + 1,
            indentPerLevel, expandCollapseButtonWidth, iconWidth,
            padding, pMaxSize, pTotalItems);

        hChild = (HTREEITEM)SendMessage(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hChild);
    }
}

RECT TreeView_ApproximateFullRect(HWND hwndTV)
{
    RECT rc = { 0 };

    HDC hdc = GetDC(hwndTV);
    HFONT hFont = (HFONT)SendMessage(hwndTV, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    int indentPerLevel = TreeView_GetIndent(hwndTV);
    int expandCollapseButtonWidth = GetSystemMetrics(SM_CXSMICON);
    int iconWidth = GetSystemMetrics(SM_CXSMICON);
    int padding = 8;

    int itemHeight = (int)SendMessage(hwndTV, TVM_GETITEMHEIGHT, 0, 0);
    if (itemHeight == 0)
        itemHeight = GetSystemMetrics(SM_CYSMICON) + 2;

    SIZE maxSize = { 0, 0 };
    int totalItems = 0;

    HTREEITEM hItem = (HTREEITEM)SendMessage(hwndTV, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    while (hItem)
    {
        MeasureItem(hwndTV, hdc, hItem, 0,
            indentPerLevel, expandCollapseButtonWidth, iconWidth,
            padding, &maxSize, &totalItems);

        hItem = (HTREEITEM)SendMessage(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
    }

    SelectObject(hdc, hOldFont);
    ReleaseDC(hwndTV, hdc);

    int verticalMargin = 4;

    int neededWidth = maxSize.cx + 4;
    int neededHeight = totalItems * itemHeight + verticalMargin;

    RECT clientRect;
    GetClientRect(hwndTV, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    int vscrollWidth = GetSystemMetrics(SM_CXVSCROLL);
    int hscrollHeight = GetSystemMetrics(SM_CYHSCROLL);

    int finalWidth = neededWidth;
    int finalHeight = neededHeight;

    if (neededHeight > clientHeight)
        finalWidth += vscrollWidth;

    if (finalWidth > clientWidth)
    {
        finalHeight += hscrollHeight;

        if (finalHeight > clientHeight && finalWidth == neededWidth)
        {
            finalWidth += vscrollWidth;
        }
    }

    int glyphSize = GetSystemMetrics(SM_CXSMICON);

    rc.left = 0;
    rc.top = 0;
    rc.right = finalWidth + glyphSize;
    rc.bottom = finalHeight;

    return rc;
}

void DrawTriangle(HDC hdc, RECT rc, bool expanded, COLORREF backgroundColor, COLORREF triangleColor)
{
    POINT pts[3];

    HBRUSH hBrush = nullptr;

    if (expanded)
    {
        pts[0] = { rc.right, rc.top + 5 };
        pts[1] = { rc.right, rc.bottom - 4 };
        pts[2] = { rc.right - 5, rc.bottom - 4 };
        hBrush = CreateSolidBrush(triangleColor);
    }
    else
    {
        pts[0] = { rc.right - 4, rc.top + 2 };
        pts[1] = { rc.right, (rc.top + rc.bottom) / 2 - 1 };
        pts[2] = { rc.right - 4, rc.bottom - 4 };
        hBrush = CreateSolidBrush(backgroundColor);
    }

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, triangleColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Polygon(hdc, pts, 3);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

mui::TreeView::TreeView()
{
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0    ;
    m_class = WC_TREEVIEW;
    m_name = L"";
    m_style = WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_FULLROWSELECT;
}

mui::UIElement::EventHandlerResult mui::TreeView::HandleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->code)
        {
        case TVN_ITEMCHANGED:
        {
            NMTVITEMCHANGE* pnmv = (NMTVITEMCHANGE*)lParam;
            if (pnmv->uStateNew & TVIS_SELECTED && SelectionChanged)
                SelectionChanged(this, { uMsg, wParam, lParam });
        }
        break;
        case NM_CUSTOMDRAW:
        {
            LPNMTVCUSTOMDRAW lpCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
            switch (lpCustomDraw->nmcd.dwDrawStage)
            {
            case CDDS_PREPAINT:
                return { TRUE,CDRF_NOTIFYITEMDRAW };

            case CDDS_ITEMPREPAINT:
            {
                HDC hdc = lpCustomDraw->nmcd.hdc;
                HTREEITEM hItem = (HTREEITEM)lpCustomDraw->nmcd.dwItemSpec;

                HTREEITEM hSelected = TreeView_GetSelection(m_hWnd);
                HTREEITEM hCurrent = (HTREEITEM)lpCustomDraw->nmcd.dwItemSpec;

                bool isSelected = (hSelected == hCurrent);

                HBRUSH selectedBackground = isSelected ? CreateSolidBrush(m_selectedColor) : CreateSolidBrush(m_backgroundColor);

                TCHAR szText[256];
                TVITEM tvi = { 0 };
                tvi.hItem = hItem;
                tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
                tvi.pszText = szText;
                tvi.cchTextMax = 256;

                TreeView_GetItem(m_hWnd, &tvi);

                RECT rc;
                if (TreeView_GetItemRect(m_hWnd, hItem, &rc, TRUE))
                {
                    RECT rcFull = rc;
                    RECT rcClient = {};
                    GetClientRect(m_hWnd, &rcClient);
                    rcFull.right = rcClient.right;
                    rcFull.left = rcClient.left;
                    FillRect(hdc, &rcFull, selectedBackground);
                }
                else
                    return { TRUE,CDRF_SKIPDEFAULT };

                HIMAGELIST hImageList = (HIMAGELIST)TreeView_GetImageList(m_hWnd, TVSIL_NORMAL);

                if (hImageList)
                    ImageList_Draw(hImageList, isSelected ? tvi.iSelectedImage : tvi.iImage, hdc, rc.left - 16, rc.top, ILD_NORMAL);

                RECT rcText = rc;
                rcText.left += 4;

                SetBkMode(hdc, TRANSPARENT);
                ::SetTextColor(hdc, m_textColor);
                DrawText(hdc, szText, -1, &rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

                DeleteObject(selectedBackground);

                if (tvi.cChildren > 0)
                {
                    bool isExpanded = (SendMessage(m_hWnd, TVM_GETITEMSTATE, (WPARAM)hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != 0;

                    const int glyphSize = 16;
                    const int extraMargin = 3;

                    int indentPixels = (int)SendMessage(m_hWnd, TVM_GETINDENT, 0, 0);

                    RECT rcExpand;
                    rcExpand.left = rc.left - indentPixels - glyphSize - extraMargin + 2;
                    rcExpand.top = rc.top + ((rc.bottom - rc.top) - glyphSize) / 2 + 2;
                    rcExpand.right = rcExpand.left + glyphSize - 2;
                    rcExpand.bottom = rcExpand.top + glyphSize - 2;

                    HBRUSH hbrush = CreateSolidBrush(TreeView_GetBkColor(m_hWnd));
                    DrawTriangle(hdc, rcExpand, isExpanded, isSelected ? m_selectedColor : m_backgroundColor,m_expandButtonColor);
                    DeleteObject(hbrush);
                }

                return { TRUE,CDRF_SKIPDEFAULT };
            }
            }
        }
        break;
        case NM_RCLICK:
            if (this->RightClick)
                RightClick(this, { uMsg,wParam,lParam });
            break;
        case NM_DBLCLK:
            if (this->DoubleClick)
                DoubleClick(this, { uMsg,wParam,lParam });
            break;
        }
    }
    break;
    }

    return { FALSE,NULL };
}

void mui::TreeView::AddChilds(const std::vector<std::shared_ptr<mui::TreeViewItem>>& items, const std::shared_ptr<mui::TreeViewItem>& root)
{
    HTREEITEM realRoot = root == NULL ? TVI_ROOT : root->m_hItem;

    for (const std::shared_ptr<mui::TreeViewItem>& item : items)
    {
        TVINSERTSTRUCT tvis = { 0 };
        tvis.hParent = realRoot;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.lParam = (LPARAM)item.get();
        tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvis.item.pszText = (LPWSTR)item->m_text.c_str();
        tvis.item.iImage = GetIconIndex(item->m_hIcon);
        tvis.item.iSelectedImage = GetIconIndex(item->m_hIcon);
        item->m_hItem = TreeView_InsertItem(GetHWND(), &tvis);

        AddChilds(item->m_children, item);
    }

    UpdateIdealSize();
}

DWORD mui::TreeView::GetIconIndex(HICON icon)
{
    if(m_iconToIndex.find(icon) == m_iconToIndex.end())
    {
        m_iconToIndex[icon] = m_iconIndex++;
        ImageList_AddIcon(m_hImageList, icon);
        TreeView_SetImageList(m_hWnd, m_hImageList, TVSIL_NORMAL);
    }

    return m_iconToIndex[icon];
}

void mui::TreeView::UpdateIdealSize()
{
    RECT rc = TreeView_ApproximateFullRect(m_hWnd);
    m_idealSize = { rc.right - rc.left, rc.bottom - rc.top };
}

void mui::TreeView::SetHWND(HWND hWnd)
{
    m_hWnd = hWnd;

    m_hImageList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 2, 2);

    TreeView_SetBkColor(m_hWnd, m_backgroundColor);
    TreeView_SetTextColor(m_hWnd, m_textColor);

    AddChilds(m_children, NULL);
}

void mui::TreeView::AddChild(const std::shared_ptr<TreeViewItem>& child)
{
    m_children.push_back(child);
    child->m_treeView = this;
    if (m_hWnd)
    {
        LockWindowUpdate(m_hWnd);
        TVINSERTSTRUCT tvis = { 0 };
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.lParam = (LPARAM)child.get();
        tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvis.item.pszText = (LPWSTR)child->m_text.c_str();
        tvis.item.iImage = GetIconIndex(child->m_hIcon);
        tvis.item.iSelectedImage = GetIconIndex(child->m_hIcon);
        child->m_hItem = TreeView_InsertItem(GetHWND(), &tvis);

        AddChilds(child->m_children, child);
        UpdateIdealSize();
        PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
        LockWindowUpdate(NULL);
    }
}

mui::TreeViewItem* mui::TreeView::GetSelectedItem()
{
    TVITEM tvi = { 0 };
    HTREEITEM hSelected = TreeView_GetSelection(m_hWnd);
    if (!hSelected)
        return NULL;
    tvi.hItem = hSelected;
    tvi.mask = TVIF_PARAM;

    TreeView_GetItem(m_hWnd, &tvi);
    return (mui::TreeViewItem*)tvi.lParam;
}

void mui::TreeView::RemoveChild(const std::shared_ptr<TreeViewItem>& child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end())
    {
        if (m_hWnd)
        {
            LockWindowUpdate(m_hWnd);
            TreeView_DeleteItem(m_hWnd, child->m_hItem);
            UpdateIdealSize();
            PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
            LockWindowUpdate(NULL);
        }

        m_children.erase(it);
    }
}

void mui::TreeView::RemoveChild(TreeViewItem* child)
{
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [child](const std::shared_ptr<TreeViewItem>& ptr) {
            return ptr.get() == child;
        });

    if (it != m_children.end())
    {
        if (m_hWnd)
        {
            LockWindowUpdate(m_hWnd);
            TreeView_DeleteItem(m_hWnd, child->m_hItem);
            UpdateIdealSize();
            PostMessage(m_parenthWnd, MUI_WM_REDRAW, NULL, NULL);
            LockWindowUpdate(NULL);
        }

        m_children.erase(it);
    }
}

size_t mui::TreeView::Count()
{
    return m_children.size();
}

void mui::TreeView::SetTextColor(COLORREF color) 
{
    m_textColor = color;
    TreeView_SetTextColor(m_hWnd, m_textColor);
}
void mui::TreeView::SetBackgroundColor(COLORREF color)
{
    m_backgroundColor = color;
    TreeView_SetBkColor(m_hWnd, m_backgroundColor);
}
void mui::TreeView::SetExpandButtonColor(COLORREF color)
{
    m_expandButtonColor = color;
}
void mui::TreeView::SetSelectedColor(COLORREF color)
{
    m_selectedColor = color;
}