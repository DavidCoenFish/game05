#pragma once
#include "json/json.hpp"

struct JSONGeometry
{
public:
    JSONGeometry();

public:
    std::_vector < D3D12_INPUT_ELEMENT_DESC > _input_element_desc;
    D3D_PRIMITIVE_TOPOLOGY _topology;
    std::_vector < float > _vertex_data;
    int _float_per_vertex;
};
void to_json(
    in_nlohmann::json&in_j,
    const JSONGeometry&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    JSONGeometry&in_p
    );
