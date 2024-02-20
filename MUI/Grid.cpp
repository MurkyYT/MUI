#include "MUI.h"
namespace MUI
{
	Grid::Grid()
	{
		this->AddColumn(0, L"*");
		this->AddRow(0, L"*");
	}
	std::vector<UIComponent*> Grid::GetComponents()
	{
		std::vector<UIComponent*> res;
		for (GridItem* itm : this->m_items) {
			res.push_back(itm->component);
		}
		return res;
	}
	void Grid::AddColumn(int x, const wchar_t* width)
	{
		if (!m_AddedCustomColumn && this->m_columns.size() > 0)
		{
			delete this->m_columns[0];
			this->m_columns.erase(this->m_columns.begin());
			m_AddedCustomColumn = TRUE;
		}
		GridColumn* col = new GridColumn();
		col->x = x;
		col->width = 0;
		col->text_width = width;
		this->m_columns.push_back(col);
	}
	void Grid::AddRow(int y, const wchar_t* height)
	{
		if (!m_AddedCustomRow && this->m_rows.size() > 0)
		{
			delete this->m_rows[0];
			this->m_rows.erase(this->m_rows.begin());
			m_AddedCustomRow = TRUE;
		}
		GridRow* row = new GridRow();
		row->y = y;
		row->height = 0;
		row->text_height = height;
		this->m_rows.push_back(row);
	}
	void Grid::AddItem(UIComponent* comp, int row, int column)
	{
		if (row == this->m_rows.size() || column == this->m_columns.size())
		{
			throw std::exception("Row index or column index is out of range\n");
			return;
		}
		GridItem* itm = new GridItem();
		itm->column = column;
		itm->row = row;
		itm->component = comp;
		this->m_columns[column]->components.push_back(comp);
		this->m_rows[row]->components.push_back(comp);
		this->m_items.push_back(itm);
		this->Reorder(comp->windowHandle);
		this->Reposition(itm);
	}
	int clip(int n, int min) {
		if (n < min)
			return min;
		return n;
	}
	void Grid::Reorder(HWND windowHandle)
	{
		RECT rect;
		if (GetClientRect(windowHandle, &rect))
		{
			std::vector<std::tuple<GridColumn*,int>> starColumns;
			std::vector<std::tuple<GridRow*, int>> starRows;
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			int freeWidth = width;
			int freeHeight = height;
			for (int i = 0; i < this->m_columns.size(); i++) 
			{
				GridColumn* o_column = this->m_columns[i];
				if (o_column->text_width == L"Auto")
				{
					o_column->width = o_column->GetBiggestWidth();
					if (i + 1 < this->m_columns.size())
						this->m_columns[i + 1]->x = o_column->width + o_column->x;
					freeWidth -= o_column->width;
				}
				else if (o_column->text_width == L"*") {
					starColumns.push_back(std::make_tuple(o_column, i));
				}
				else
				{
					if (std::wstring(o_column->text_width).find_first_not_of(L"0123456789") == std::string::npos) {
						size_t widthLen = wcslen(o_column->text_width) * sizeof(wchar_t);
						wchar_t* s = (wchar_t*)malloc(widthLen + 1);
						wcscpy_s(s, widthLen , o_column->text_width);
						s[widthLen + 1] = 0;
						int lenInt = _wtoi(s);
						o_column->width = lenInt;
						if (i + 1 < this->m_columns.size())
							this->m_columns[i + 1]->x = o_column->width + o_column->x;
						freeWidth -= lenInt;
					}
				}
			}
			if (starColumns.size() > 0) 
			{
				int eachStarWidth = freeWidth / (int)starColumns.size();
				for (int i = 0; i < starColumns.size(); i++)
				{
					std::tuple<GridColumn*, int> column = starColumns[i];
					GridColumn* o_column = std::get<0>(column);
					o_column->width = eachStarWidth;
					size_t index = std::get<1>(column);
					if (index + 1 < this->m_columns.size())
						this->m_columns[index + 1]->x = o_column->width + o_column->x;
				}
			}
			for (int i = 0; i < this->m_rows.size(); i++) {
				GridRow* o_row = this->m_rows[i];
				if (o_row->text_height == L"Auto")
				{
					o_row->height = o_row->GetBiggestHeight();
					if (i + 1 < this->m_rows.size())
						this->m_rows[i + 1]->y = o_row->height + o_row->y;
					freeHeight -= o_row->height;
				}
				if (o_row->text_height == L"*") {
					starRows.push_back(std::make_tuple(o_row, i));
				}
				else
				{
					if (std::wstring(o_row->text_height).find_first_not_of(L"0123456789") == std::string::npos) {
						size_t heightLen = wcslen(o_row->text_height) * sizeof(wchar_t);
						wchar_t* s = (wchar_t*)malloc(heightLen + 1);
						wcscpy_s(s, heightLen, o_row->text_height);
						s[heightLen + 1] = 0;
						int lenInt = _wtoi(s);
						o_row->height = lenInt;
						if (i + 1 < this->m_rows.size())
							this->m_rows[i + 1]->y = o_row->height + o_row->y;
						freeHeight -= lenInt;
					}
				}
			}
			if (starRows.size() > 0)
			{
				int eachStarHeight = freeHeight / (int)starRows.size();
				for (int i = 0; i < starRows.size(); i++)
				{
					std::tuple<GridRow*, int> column = starRows[i];
					GridRow* o_row = std::get<0>(column);
					o_row->height = eachStarHeight;
					size_t index = std::get<1>(column);
					if (index + 1 < this->m_rows.size())
						this->m_rows[index + 1]->y = o_row->height + o_row->y;
				}
			}
			GridRow* row = this->m_rows[m_rows.size() - 1];
			GridColumn* col = this->m_columns[m_columns.size() - 1];
			if (row->text_height == L"*")
				row->height = height - row->y;
			if (col->text_width == L"*")
				col->width = width - col->x;
		}
	}
	void Grid::Reposition(GridItem* itm)
	{
		RECT rect;
		if (GetClientRect(itm->component->windowHandle, &rect))
		{
			GridColumn* o_column = this->m_columns[itm->column];
			GridRow* o_row = this->m_rows[itm->row];
			UIComponent* comp = itm->component;
			int width = max(o_column->width, 0);
			int height = max(o_row->height, 0);
			for (size_t i = 1; i < itm->component->columnSpan && i < this->m_columns.size(); i++)
				width += this->m_columns[itm->column + i]->width;
			for (size_t i = 1; i < itm->component->rowSpan && i < this->m_rows.size(); i++)
				height += this->m_rows[itm->row + i]->height;
			comp->x = o_column->x;
			comp->y = o_row->y;
			comp->reposition(height, width);
		}
	}
}