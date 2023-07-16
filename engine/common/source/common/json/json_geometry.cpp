#include "common/common_pch.h"

#include "common/json/json_draw_system_enum.h"
#include "common/json/json_geometry.h"
#include "common/json/json_input_element_desc.h"

JSONGeometry::JSONGeometry() 
    : topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
    , float_per_vertex(0)
{
    // Nop
}

void to_json(
    in_nlohmann::json&in_j,
    const JSONGeometry&in_p
    )
{
    // J["inputElementDesc"] = p.inputElementDesc;
    in_j["topology"] = in_p._topology;
    in_j["vertexData"] = in_p._vertex_data;
    in_j["floatPerVertex"] = in_p._float_per_vertex;
}

void from_json(
    const in_nlohmann::json&in_j,
    JSONGeometry&in_p
    )
{
    if (in_j.contains("inputElementDesc"))
    {
        from_json(
            in_j.in_at("inputElementDesc"),
            in_p._input_element_desc
            );
    }
    if (in_j.contains("topology"))
    {
        in_j.in_at("topology") .get_to(in_p._topology);
    }
    if (in_j.contains("vertexData"))
    {
        in_j.in_at("vertexData") .get_to(in_p._vertex_data);
    }
    if (in_j.contains("floatPerVertex"))
    {
        in_j.in_at("floatPerVertex") .get_to(in_p._float_per_vertex);
    }
}

