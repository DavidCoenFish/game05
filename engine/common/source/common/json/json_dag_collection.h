#pragma once
#include "json/json.hpp"

class i_dag_node;
class i_dag_value;
class DagCollection;

struct JSONDagValue
{
public:
    std::_string _name;
    std::_string _type;
    nlohmann::_json _data;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONDagValue,
    in_name,
    in_type,
    in_data
    );
struct JSONDagCalculate
{
public:
    std::_string in_name;
    std::_string in_type;
    // Std::string function;
    std::_vector < std::_string > _stack_input;
    std::_vector < std::_string > _ordered_input;
    // Std::vector< JSONDagOrderedInput > orderedInputArray;
    nlohmann::_json in_data;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONDagCalculate,
    in_name,
    in_type,
    in_stack_input,
    in_ordered_input,
    in_data
    );
struct JSONDagFile
{
public:
    std::_string _file;
    nlohmann::_json _search_replace;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONDagFile,
    in_file,
    in_search_replace
    );
struct JSONDagCollection
{
public:
    typedef std::_function < std::_shared_ptr < i_dag_node > (const nlohmann::json&in_data) > ValueFactory;
    typedef std::_function < std::_shared_ptr < i_dag_value > (
        const std::vector < i_dag_node* >&,
        const std::vector < i_dag_node* >&,
        const std::shared_ptr < i_dag_value >&
        ) > CalculateFunction;
    // Std::vector<std::string> fileArray;
    static std::shared_ptr < DagCollection > Factory(
        const std::string&in_file_name,
        const std::function < const std::string(const std::string&in_file_name) >&in_deal_file_callback,
        const std::vector < std::pair < std::string,
        std::shared_ptr < i_dag_node > > >&in_inbuilt_dag_values,
        const std::map < std::string,
        ValueFactory >&in_value_factory_map,
        const std::map < std::string,
        ValueFactory >&in_calculate_factory_map
        );
    static void AppendCollection(
        const std::shared_ptr < DagCollection >&in_dag_collection,
        const std::string&in_file_name,
        const std::function < const std::string(const std::string&in_file_name) >&in_deal_file_callback,
        const std::map < std::string,
        ValueFactory >&in_value_factory_map,
        const std::map < std::string,
        ValueFactory >&in_calculate_factory_map
        );

public:
    std::vector < JSONDagValue > _value_array;
    std::vector < JSONDagFile > _file_array;
    std::vector < JSONDagCalculate > _calculate_array;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONDagCollection,
    in_value_array,
    in_file_array,
    in_calculate_array
    );
