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
	m_rows.push_back(RowDefinition{ GridLength(1.0f, GridUnitType::Star) });
	m_columns.push_back(ColumnDefinition{ GridLength(1.0f, GridUnitType::Star) });
}

std::vector<mui::RowDefinition>& mui::Grid::RowDefinitions() { return m_rows; }
std::vector<mui::ColumnDefinition>& mui::Grid::ColumnDefinitions() { return m_columns; }

void mui::Grid::AddChild(const std::shared_ptr<UIElement>& element, size_t row, size_t column) 
{
	if (row >= m_rows.size() || row < 0)
		return;

	if (column >= m_columns.size() || column < 0)
		return;

	m_elementGridPosition[element.get()] = {row, column};

	m_collection.Add(element);
}

void mui::Grid::RemoveChild(const std::shared_ptr<UIElement>& element)
{
	auto it = m_elementGridPosition.find(element.get());

	if(it != m_elementGridPosition.end())
	{
		m_elementGridPosition.erase(it);
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
	long totalHeight = m_availableSize.bottom - m_availableSize.top;
	double totalStar = 0;
	long usedHeight = 0;
	for (size_t i = 0; i < m_rows.size(); ++i)
	{
		auto& row = m_rows[i];
		if (row.height.unit == GridUnitType::Pixel)
			row.actualHeight = (double)row.height.value;
		else if (row.height.unit == GridUnitType::Auto)
		{
			size_t maxHeight = 0;
			for (const auto& item : m_elementGridPosition)
			{
				if (item.second.first == i)
					maxHeight = max(maxHeight, item.first->GetMinHeight());
			}
			row.actualHeight = (double)maxHeight;
		}
		else if (row.height.unit == GridUnitType::Star)
		{
			totalStar += row.height.value;
			row.actualHeight = 0;
		}
		usedHeight += (long)row.actualHeight;
	}
	long remaining = totalHeight - usedHeight;
	if (totalStar > 0)
	{
		for (auto& row : m_rows)
			if (row.height.unit == GridUnitType::Star)
				row.actualHeight = (double)((row.height.value / totalStar) * remaining);
	}
}

void mui::Grid::CalculateColumnWidths()
{
	long totalWidth = m_availableSize.right - m_availableSize.left;
	double totalStar = 0;
	long usedWidth = 0;
	for (size_t i = 0; i < m_columns.size(); ++i)
	{
		auto& col = m_columns[i];
		if (col.width.unit == GridUnitType::Pixel)
			col.actualWidth = (double)col.width.value;
		else if (col.width.unit == GridUnitType::Auto)
		{
			size_t maxWidth = 0;
			for (const auto& item : m_elementGridPosition)
			{
				if (item.second.second == i)
					maxWidth = max(maxWidth, item.first->GetMinWidth());
			}
			col.actualWidth = (double)maxWidth;
		}
		else if (col.width.unit == GridUnitType::Star)
		{
			totalStar += col.width.value;
			col.actualWidth = 0;
		}
		usedWidth += (long)col.actualWidth;
	}
	long remaining = totalWidth - usedWidth;
	if (totalStar > 0)
	{
		for (auto& col : m_columns)
			if (col.width.unit == GridUnitType::Star)
				col.actualWidth = (double)((col.width.value / totalStar) * remaining);
	}
}

void mui::Grid::PerformLayout()
{
	CalculateRowHeights();
	CalculateColumnWidths();

	for (const auto& el : m_collection.Items())
	{
		size_t row = 0, col = 0;
		auto it = m_elementGridPosition.find(el.get());
		if (it != m_elementGridPosition.end()) 
		{
			row = it->second.first;
			col = it->second.second;
		}
		else
			continue;

		long x = 0;
		for (size_t i = 0; i < col; ++i)
			x += (long)m_columns[i].actualWidth;

		long y = 0;
		for (size_t i = 0; i < row; ++i)
			y += (long)m_rows[i].actualHeight;

		SetWindowPos(el->GetHWND(), NULL, x, y, (int)m_columns[col].actualWidth, (int)m_rows[row].actualHeight, SWP_NOZORDER);
		InvalidateRect(el->GetHWND(), NULL, TRUE);
	}
}

size_t mui::Grid::GetMinWidth() { return m_availableSize.right - m_availableSize.left; }
size_t mui::Grid::GetMinHeight() { return m_availableSize.bottom - m_availableSize.top; }
size_t mui::Grid::GetMaxWidth() { return GetMinWidth(); }
size_t mui::Grid::GetMaxHeight() { return GetMinHeight(); }

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