#include "Grid.h"

#include <stdexcept>

mui::Grid::Grid()
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Grid::WindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszClassName = L"MUI_Grid";

	if (!RegisterClassEx(&wcex))
	{
		DWORD error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			throw std::runtime_error("Class creation failed");
	}

	m_class = L"MUI_Grid";
	m_style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	m_x = m_y = 0;
	m_subclass = FALSE;
}

std::vector<mui::RowDefinition>& mui::Grid::RowDefinitions() { return m_rows; }
std::vector<mui::ColumnDefinition>& mui::Grid::ColumnDefinitions() { return m_columns; }

void mui::Grid::AddChild(const std::shared_ptr<UIElement>& element) 
{
	if (m_elementGridPlacement.find(element.get()) == m_elementGridPlacement.end())
		m_elementGridPlacement[element.get()] = { 0, 0, 1, 1 };

	m_collection.Add(element);
}

void mui::Grid::SetRow(const std::shared_ptr<UIElement>& element, size_t row)
{
	size_t realRow = min(m_rows.size(), row);
	if (m_elementGridPlacement.find(element.get()) == m_elementGridPlacement.end())
		m_elementGridPlacement[element.get()] = { realRow, 0, 1, 1 };
	else
		m_elementGridPlacement[element.get()].row = realRow;
}

void mui::Grid::SetRowSpan(const std::shared_ptr<UIElement>& element, size_t rowSpan)
{
	if (m_elementGridPlacement.find(element.get()) == m_elementGridPlacement.end())
		m_elementGridPlacement[element.get()] = { 0, 0, rowSpan, 1 };
	else
		m_elementGridPlacement[element.get()].rowSpan = rowSpan;
}

void mui::Grid::SetColumn(const std::shared_ptr<UIElement>& element, size_t column)
{
	size_t realColumn = min(m_columns.size(), column);
	if (m_elementGridPlacement.find(element.get()) == m_elementGridPlacement.end())
		m_elementGridPlacement[element.get()] = { 0, realColumn, 1, 1 };
	else
		m_elementGridPlacement[element.get()].column = realColumn;
}

void mui::Grid::SetColumnSpan(const std::shared_ptr<UIElement>& element, size_t columnSpan)
{
	if (m_elementGridPlacement.find(element.get()) == m_elementGridPlacement.end())
		m_elementGridPlacement[element.get()] = { 0, 0, 1, columnSpan };
	else
		m_elementGridPlacement[element.get()].columnSpan = columnSpan;
}

void mui::Grid::RemoveChild(const std::shared_ptr<UIElement>& element)
{
	auto it = m_elementGridPlacement.find(element.get());

	if(it != m_elementGridPlacement.end())
	{
		m_elementGridPlacement.erase(it);
		m_collection.Remove(element);
	}

}

void mui::Grid::SetParentHWND(HWND p_hWnd) { m_parenthWnd = p_hWnd; }
void mui::Grid::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
	for (const auto& element : m_collection.Items())
		element->Initialize(m_hWnd, element->GetID(), m_collection.GetFontHandle());

	m_collection.SetHWND(hWnd);
}

void mui::Grid::CalculateRowHeights()
{
	long totalHeight = (long)m_lastRequestedHeight;
	double totalStar = 0;
	long usedHeight = 0;
	std::vector<RowDefinition*> m_star;

	for (size_t i = 0; i < m_rows.size(); ++i)
	{
		auto& row = m_rows[i];
		if (row.height.unit == GridUnitType::Pixel)
			row.actualHeight = (double)row.height.value;
		else if (row.height.unit == GridUnitType::Auto)
		{
			size_t maxHeight = 0;
			for (const auto& element : m_collection.Items())
			{
				const auto& item = m_elementGridPlacement[element.get()];
				if (item.row == i)
					maxHeight = max(maxHeight, element->GetMinHeight());
			}
			row.actualHeight = (double)maxHeight;
		}
		else if (row.height.unit == GridUnitType::Star)
		{
			totalStar += row.height.value;
			row.actualHeight = 0;
			m_star.push_back(&row);
		}
		usedHeight += (long)row.actualHeight;
	}
	long remaining = totalHeight - usedHeight;
	if (totalStar > 0)
	{
		for (auto& row : m_star)
			row->actualHeight = (double)((row->height.value / totalStar) * remaining);
	}
}

void mui::Grid::CalculateColumnWidths()
{
	long totalWidth = (long)m_lastRequestedWidth;
	double totalStar = 0;
	long usedWidth = 0;
	std::vector<ColumnDefinition*> m_star;

	for (size_t i = 0; i < m_columns.size(); ++i)
	{
		auto& col = m_columns[i];
		if (col.width.unit == GridUnitType::Pixel)
			col.actualWidth = (double)col.width.value;
		else if (col.width.unit == GridUnitType::Auto)
		{
			size_t maxWidth = 0;
			for (const auto& element : m_collection.Items())
			{
				const auto& item = m_elementGridPlacement[element.get()];
				if (item.column == i)
					maxWidth = max(maxWidth, element->GetMinWidth());
			}
			col.actualWidth = (double)maxWidth;
		}
		else if (col.width.unit == GridUnitType::Star)
		{
			totalStar += col.width.value;
			col.actualWidth = 0;
			m_star.push_back(&col);
		}
		usedWidth += (long)col.actualWidth;
	}
	long remaining = totalWidth - usedWidth;
	if (totalStar > 0)
	{
		for (auto& col : m_star)
			col->actualWidth = (double)((col->width.value / totalStar) * remaining);
	}
}

void mui::Grid::PerformLayout()
{
	CalculateRowHeights();
	CalculateColumnWidths();

	for (const auto& el : m_collection.Items())
	{
		size_t row = 0, col = 0, colSpan = 1, rowSpan = 1;
		auto it = m_elementGridPlacement.find(el.get());
		if (it != m_elementGridPlacement.end()) 
		{
			row = it->second.row;
			rowSpan = it->second.rowSpan;
			col = it->second.column;
			colSpan = it->second.columnSpan;
		}
		else
			continue;

		double width = 0, height = 0;

		long x = 0;
		for (size_t i = 0; i < col; ++i)
			x += (long)m_columns[i].actualWidth;

		long y = 0;
		for (size_t i = 0; i < row; ++i)
			y += (long)m_rows[i].actualHeight;

		for (size_t i = col; i < col + colSpan && i < m_columns.size(); ++i)
			width += m_columns[i].actualWidth;
		for (size_t i = row; i < row + rowSpan && i < m_rows.size(); ++i)
			height += m_rows[i].actualHeight;


		el->SetAvailableSize({ 0,0,
			m_columns.size() == 0 ? m_availableSize.right - m_availableSize.left : (int)width,
			m_rows.size() == 0 ? m_availableSize.bottom - m_availableSize.top : (int)height });
		SetWindowPos(el->GetHWND(), NULL, x + (int)el->GetX(), y + (int)el->GetY(), (int)el->GetMaxWidth(), (int)el->GetMaxHeight(), SWP_NOZORDER);
		InvalidateRect(el->GetHWND(), NULL, TRUE);
	}
}

size_t mui::Grid::GetMinWidth()
{
	size_t minWidth = 0;
	std::vector<size_t> columnToWidth;
	columnToWidth.resize(m_columns.size() == 0 ? 1 : m_columns.size());
	for (const auto& el : m_collection.Items())
	{
		size_t col = 0;
		auto it = m_elementGridPlacement.find(el.get());
		if (it != m_elementGridPlacement.end())
			col = it->second.column;

		if (columnToWidth[col] < el->GetMinWidth())
			columnToWidth[col] = el->GetMinWidth();
	}

	for (size_t size : columnToWidth)
		minWidth += size;

	m_lastRequestedWidth = minWidth;
	return minWidth;
}
size_t mui::Grid::GetMinHeight()
{
	size_t minHeight = 0;
	std::vector<size_t> rowToHeight;
	rowToHeight.resize(m_rows.size() == 0 ? 1 : m_rows.size());
	for (const auto& el : m_collection.Items())
	{
		size_t row = 0;
		auto it = m_elementGridPlacement.find(el.get());
		if (it != m_elementGridPlacement.end())
			row = it->second.row;

		if(rowToHeight[row] < el->GetMinHeight())
			rowToHeight[row] = el->GetMinHeight();
	}

	for (size_t size : rowToHeight)
		minHeight += size;

	m_lastRequestedHeight = minHeight;
	return minHeight;
}
size_t mui::Grid::GetMaxWidth() 
{ 
	m_lastRequestedWidth = m_availableSize.right - m_availableSize.left;
	return m_availableSize.right - m_availableSize.left; 
}
size_t mui::Grid::GetMaxHeight()
{
	m_lastRequestedHeight = m_availableSize.bottom - m_availableSize.top;
	return m_availableSize.bottom - m_availableSize.top; 
}

mui::UIElement::EventHandlerResult mui::Grid::HandleEvent(UINT, WPARAM, LPARAM) { return { FALSE, 0 }; }
LRESULT CALLBACK mui::Grid::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Grid* grid = (Grid*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		grid = reinterpret_cast<Grid*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)grid);
		grid->m_hWnd = hWnd;
	}

	if (grid)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
		{
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
		}
		break;
		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			RECT rc;
			GetClientRect(hWnd, &rc);
			HBRUSH hBrush = CreateSolidBrush(grid->m_backgroundColor);
			FillRect(hdc, &rc, hBrush);

			DeleteObject(hBrush);
			return 1;
		}
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
			SetFocus(hWnd);
			break;
		case WM_KEYDOWN:
			PostMessage(grid->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case WM_KEYUP:
			PostMessage(grid->m_parenthWnd, uMsg, wParam, lParam);
			break;
		case MUI_WM_REDRAW:
			PostMessage(grid->m_parenthWnd, uMsg, wParam, lParam);
		case WM_SIZE:
			grid->PerformLayout();
			break;
		case WM_COMMAND:
		{
			if (grid->m_collection.IDExists((DWORD)LOWORD(wParam)))
			{
				EventHandlerResult res = grid->m_collection.ItemByID((DWORD)LOWORD(wParam))->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_NOTIFY:
		{
			if (grid->m_collection.IDExists((DWORD)((LPNMHDR)lParam)->idFrom))
			{
				EventHandlerResult res = grid->m_collection.ItemByID((DWORD)((LPNMHDR)lParam)->idFrom)->HandleEvent(uMsg, wParam, lParam);
				if (res.returnVal)
					return res.value;
			}
		}
		break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
		{
			HWND hwnd = (HWND)lParam;
			if (hwnd != NULL) {
				int controlId = GetDlgCtrlID(hwnd);
				if (grid->m_collection.IDExists(controlId))
				{
					EventHandlerResult res = grid->m_collection.ItemByID(controlId)->HandleEvent(uMsg, wParam, lParam);
					if (res.returnVal)
						return res.value;
				}
			}
		}
		break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}