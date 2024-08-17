#include "MUI.h"
#include "Divider.h"
#include <cwchar>
#include <memory>
#include <wingdi.h>

namespace MUI
{
	/*Grid::~Grid()
	{
		for (size_t i = 0; i < m_rows.size(); i++)
			m_rows[i].reset();
		for (size_t i = 0; i < m_columns.size(); i++)
			m_columns[i].reset();
		for (size_t i = 0; i < m_items.size(); i++)
			m_items[i].reset();
		if(!m_items.empty())
			m_items.clear();
		if (!m_rows.empty())
			m_rows.clear();
		if (!m_columns.empty())
			m_columns.clear();
	}*/
	Grid::Grid()
	{
		this->AddColumn(0, L"*");
		this->AddRow(0, L"*");
	}

	std::vector<std::shared_ptr<Divider>> Grid::GetDividers()
	{
	    std::vector<std::shared_ptr<Divider>> res;
    	for (auto rowCol : this->rowColToDivider) {
    	    for (auto colDiv : this->rowColToDivider[rowCol.first]) {
    			res.push_back(this->rowColToDivider[rowCol.first][colDiv.first]);
    		}
    	}
        return res;
	}
	std::vector<UIComponent*> Grid::GetComponents()
	{
		std::vector<UIComponent*> res;
		for (std::shared_ptr<GridItem> itm : this->m_items) {
			res.push_back(itm->component);
		}
		return res;
	}

	void Grid::AddDivider(int column, int row, BOOL isVertical)
	{
	    std::shared_ptr<Divider> divider = std::shared_ptr<Divider>(new Divider());
		if (row == this->m_rows.size() || column == this->m_columns.size())
		{
			throw std::exception("Row index or column index is out of range\n");
			return;
		}
	    if(rowColToDivider[row].find(column) != rowColToDivider[row].end())
		{
            throw std::exception("Only one divider per column row combo is permitted\n");
		    return;
		}
		divider->row = row;
		divider->column = column;
		rowColToDivider[row][column] = divider;
	}

	void Grid::AddColumn(int x, const wchar_t* width)
	{
		if (!m_AddedCustomColumn && this->m_columns.size() > 0)
		{
			this->m_columns[0].reset();
			this->m_columns.erase(this->m_columns.begin());
			m_AddedCustomColumn = TRUE;
		}
		GridColumn* col = new GridColumn();
		col->x = x;
		col->perc = 1;
		col->width = 0;
		col->text_width = width;
		this->m_columns.push_back(std::shared_ptr<GridColumn>(col));
	}
	void Grid::AddRow(int y, const wchar_t* height)
	{
		if (!m_AddedCustomRow && this->m_rows.size() > 0)
		{
			this->m_rows[0].reset();
			this->m_rows.erase(this->m_rows.begin());
			m_AddedCustomRow = TRUE;
		}
		GridRow* row = new GridRow();
		row->y = y;
		row->height = 0;
		row->text_height = height;
		this->m_rows.push_back(std::shared_ptr<GridRow>(row));
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
		this->m_items.push_back(std::shared_ptr<GridItem>(itm));
		this->Reorder(comp->windowHandle);
		this->Reposition(itm);
	}
	void Grid::Reorder(HWND windowHandle,BOOL firstRun)
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
				GridColumn* o_column = this->m_columns[i].get();
				if (wcscmp(o_column->text_width, L"Auto") == 0)
				{
					o_column->width = o_column->GetBiggestWidth();
					if (i + 1 < this->m_columns.size())
						this->m_columns[i + 1]->x = o_column->width + o_column->x;
					freeWidth -= o_column->width;
				}
				else if (wcscmp(o_column->text_width, L"*")  == 0) {
					starColumns.push_back(std::make_tuple(o_column, i));
				}
				else
				{
					if (std::wstring(o_column->text_width).find_first_not_of(L"0123456789") == std::string::npos) {
						size_t widthLen = wcslen(o_column->text_width);
						wchar_t* s = (wchar_t*)malloc(widthLen * sizeof(wchar_t) + 2);
						wcscpy_s(s, widthLen + 1, o_column->text_width);
						int lenInt = _wtoi(s);
						o_column->width = lenInt;
						if (i + 1 < this->m_columns.size())
							this->m_columns[i + 1]->x = o_column->width + o_column->x;
						freeWidth -= lenInt;
						free(s);
					}
				}
			}
			if (starColumns.size() > 0)
			{
				double maxPerc = 0;
				for (int i = 0; i < starColumns.size(); i++)
				{
					std::tuple<GridColumn*, int> column = starColumns[i];
					GridColumn* o_column = std::get<0>(column);
					maxPerc += o_column->perc;
				}
				for (int i = 0; i < starColumns.size(); i++)
				{
					std::tuple<GridColumn*, int> column = starColumns[i];
					GridColumn* o_column = std::get<0>(column);
					o_column->width = (o_column->perc / maxPerc) * freeWidth;
					size_t index = std::get<1>(column);
					if (index + 1 < this->m_columns.size())
						this->m_columns[index + 1]->x = o_column->width + o_column->x;
				}
			}
			for (int i = 0; i < this->m_rows.size(); i++) {
				GridRow* o_row = this->m_rows[i].get();
				if (wcscmp(o_row->text_height, L"Auto") == 0)
				{
					o_row->height = o_row->GetBiggestHeight();
					if (i + 1 < this->m_rows.size())
						this->m_rows[i + 1]->y = o_row->height + o_row->y;
					freeHeight -= o_row->height;
				}
				if (wcscmp(o_row->text_height, L"*")  == 0) {
					starRows.push_back(std::make_tuple(o_row, i));
				}
				else
				{
					if (std::wstring(o_row->text_height).find_first_not_of(L"0123456789") == std::string::npos) {
						size_t heightLen = wcslen(o_row->text_height) ;
						wchar_t* s = (wchar_t*)malloc(heightLen * sizeof(wchar_t) + 2);
						wcscpy_s(s, heightLen + 1, o_row->text_height);
						int lenInt = _wtoi(s);
						o_row->height = lenInt;
						if (i + 1 < this->m_rows.size())
							this->m_rows[i + 1]->y = o_row->height + o_row->y;
						freeHeight -= lenInt;
						free(s);
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
			GridRow* row = this->m_rows[m_rows.size() - 1].get();
			GridColumn* col = this->m_columns[m_columns.size() - 1].get();
			if (wcscmp(row->text_height, L"*")  == 0)
				row->height = height - row->y;
			if (wcscmp(col->text_width, L"*")  == 0)
				col->width = width - col->x;

			if(!firstRun)
				CalculateStarPercentages();

			for (size_t row = 0; row < m_rows.size(); row++)
			{
				for (size_t column = 0; column < m_columns.size(); column++)
				{
					if (rowColToDivider[row].find(column) != rowColToDivider[row].end()) {
						rowColToDivider[row][column]->x = m_columns[column]->x;
						rowColToDivider[row][column]->y = m_rows[row]->y;

						SetWindowPos(rowColToDivider[row][column]->hwnd,NULL, rowColToDivider[row][column]->x, rowColToDivider[row][column]->y, rowColToDivider[row][column]->width, rowColToDivider[row][column]->height, SWP_SHOWWINDOW);
					}
				}
			}
		}
	}

	void Grid::CalculateStarPercentages()
	{
		std::vector<std::shared_ptr<Divider>> dividers = GetDividers();
		for (std::shared_ptr<Divider> div : dividers)
		{
			if (!div->strct)
				continue;
			if(div->isVertical)
			{
			}
			else
			{
				if(wcscmp(m_columns[div->column]->text_width,L"*") == 0
					&& div->column - 1 >= 0
					&& wcscmp(m_columns[div->column-1]->text_width, L"*") == 0)
				{
					GridColumn* right = m_columns[div->column].get(), *left = m_columns[div->column - 1].get();
					size_t fullWidth = right->width + left->width;
					LONG newPos = Divider_GetMovement(div->strct) + div->start.x;

					if(newPos < left->x)
						newPos = left->x;
					else if(newPos > right->x + right->width - DIVIDER_SIZE)
						newPos = right->x + right->width - DIVIDER_SIZE;

					double leftSide = newPos - left->x;
					double rightSide = (left->x + fullWidth) - newPos;

					left->perc = leftSide / fullWidth;
					right->perc = rightSide / fullWidth;

					right->x = leftSide + left->x;
					right->width = rightSide;
					left->width = leftSide;
				}
			}
		}
	}

	void Grid::Reposition(GridItem* itm)
	{
		RECT rect;
		UIComponent* comp = itm->component;
		if (GetClientRect(itm->component->windowHandle, &rect))
		{
			GridColumn* o_column = this->m_columns[itm->column].get();
			GridRow* o_row = this->m_rows[itm->row].get();
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
		InvalidateRect(itm->component->windowHandle, NULL, FALSE);
		UpdateWindow(itm->component->windowHandle);
	}
}
