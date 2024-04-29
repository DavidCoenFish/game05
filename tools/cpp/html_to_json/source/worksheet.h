#pragma once

#include <memory>
#include <string>
#include <vector>

/// bare bones representation of the data in a spreadsheet worksheet
class Worksheet
{
public:
    void AppendRow(const std::vector<std::string>& in_row_data);

    const int GetRowCount() const;
    const int GetRowCellCount(const int in_row_index) const;
    const std::string& GetCell(const int in_row_index, const int in_cell_index) const;

private:
    typedef std::vector<std::string> Row;
    std::vector<std::shared_ptr<Row>> _row_data = {};

};
