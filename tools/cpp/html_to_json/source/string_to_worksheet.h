#pragma once

#include <memory>
#include <map>
#include <string>

class Worksheet;

namespace StringToWorksheet
{
    const bool DealSourceData(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_source_data);

}
