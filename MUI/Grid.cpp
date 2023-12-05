#include "MUI.h"
namespace MUI
{
	Grid::Grid()
	{
		this->AddColumn(0, L"Auto");
		this->AddRow(0, L"Auto");
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
		GridColumn* col = new GridColumn();
		col->x = x;
		col->width = 0;
		col->text_width = width;
		this->m_columns.push_back(col);
	}
	void Grid::AddRow(int y, const wchar_t* height)
	{
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
			OutputDebugString(L"Row index or column index is out of range");
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
			for (int i = 0; i < this->m_columns.size(); i++) {
				GridColumn* o_column = this->m_columns[i];
			
				if (o_column->text_width == L"Auto")
				{
					o_column->width = o_column->GetBiggestWidth();
					for (int j =  i + 1; j < this->m_columns.size(); j++)
						this->m_columns[j]->x += o_column->width - clip((this->m_columns[j]->x - o_column->x),0);
				}
				if (o_column->text_width == L"*") {
					starColumns.push_back(std::make_tuple(o_column, i));
					o_column->width = o_column->GetBiggestWidth();
				}
			}
			for(std::tuple<GridColumn*,int> star : starColumns)
			{
				GridColumn* o_column = std::get<0>(star);
				int othersWidth = 0;
				for (int j = std::get<1>(star) + 1; j < this->m_columns.size(); j++)
					othersWidth += this->m_columns[j]->width;
				o_column->width = rect.right - rect.left - o_column->x - othersWidth;
				for (int j = std::get<1>(star) + 1; j < this->m_columns.size(); j++)
					this->m_columns[j]->x += o_column->width;
			}
			std::vector<std::tuple<GridRow*, int>> starRows;
			for (int i = 0; i < this->m_rows.size(); i++) {
				GridRow* o_row = this->m_rows[i];
				if (o_row->text_height == L"Auto")
				{
					o_row->height = o_row->GetBiggestHeight();
					for (int j = i + 1; j < this->m_rows.size(); j++)
						this->m_rows[j]->y += o_row->height - clip((this->m_rows[j]->y - o_row->y),0);
				}
				if (o_row->text_height == L"*") {
					starRows.push_back(std::make_tuple(o_row, i));
					o_row->height = o_row->GetBiggestHeight();
				}
			}
			for (std::tuple<GridRow*, int> star : starRows)
			{
				GridRow* o_row = std::get<0>(star);
				int othersHeight = 0;
				for (int j = std::get<1>(star) + 1; j < this->m_rows.size(); j++)
					othersHeight += this->m_rows[j]->height;
				o_row->height = rect.bottom - rect.top - o_row->y - othersHeight;
				for (int j = std::get<1>(star) + 1; j < this->m_rows.size(); j++)
					this->m_rows[j]->y += o_row->height;
			}
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
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
			comp->x = o_column->x;
			comp->y = o_row->y;
			comp->reposition(o_row->height, o_column->width);
		}
	}
}