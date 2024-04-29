#include "worksheet.h"

void Worksheet::AppendRow(const std::vector<std::string>& in_row_data)
{
    _row_data.push_back(std::make_shared<Row>(in_row_data));
    return;
}

const int Worksheet::GetRowCount() const
{
    return static_cast<int>(_row_data.size());
}

const int Worksheet::GetRowCellCount(const int in_row_index) const
{
    if ((0 <= in_row_index) && (in_row_index < GetRowCount()))
    {
        return static_cast<int>(_row_data[in_row_index]->size());
    }
    return 0;
}

const std::string& Worksheet::GetCell(const int in_row_index, const int in_cell_index) const
{
    if ((0 <= in_row_index) && (in_row_index < GetRowCount()))
    {
        const auto& row = *_row_data[in_row_index];
        if ((0 <= in_cell_index) && (in_cell_index < static_cast<int>(row.size())))
        {
            return row[in_cell_index];
        }
    }
    const static std::string s_empty = {};
    return s_empty;
}
