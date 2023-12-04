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
		this->m_items.push_back(itm);
		this->Reposition(itm);
	}
	void Grid::Reposition(GridItem* itm)
	{
		GridColumn* o_column = this->m_columns[itm->column];
		GridRow* o_row = this->m_rows[itm->row];
		UIComponent* comp = itm->component;
		if (o_column->text_width == L"Auto")
		{
			if (comp->GetFullWidth() > o_column->width)
				o_column->width = comp->GetFullWidth();
			for (int i = itm->column + 1; i < this->m_columns.size(); i++)
				this->m_columns[i]->x += o_column->width - (this->m_columns[i]->x - o_column->x);
		}
		if (o_row->text_height == L"Auto")
		{
			if (comp->GetFullHeight() > o_row->height)
				o_row->height = comp->GetFullHeight();
			for (int i = itm->row + 1; i < this->m_rows.size(); i++)
				this->m_rows[i]->y += o_row->height - (this->m_rows[i]->y - o_row->y);
		}
		RECT rect;
		if (GetClientRect(comp->windowHandle, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			GridRow* row = this->m_rows[m_rows.size() - 1];
			GridColumn* col = this->m_columns[m_columns.size() - 1];
			row->height = height - row->y;
			col->width = width - col->x;
		}
		comp->x = o_column->x;
		comp->y = o_row->y;
		comp->reposition(o_row->height, o_column->width);
	}
}