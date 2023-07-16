#pragma once
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/json/json_constant_buffer_info.h"
#include "common/json/json_draw_system_enum.h"
#include "common/json/json_shader_resource_info.h"
#include "json/json.hpp"


void to_json(
    in_nlohmann::json&in_j,
    const ShaderPipelineStateData&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    ShaderPipelineStateData&in_p
    );
struct JSONShader
{
public:
    ShaderPipelineStateData _pipeline_state;
    std::_string _vertex_shader;
    std::_string _geometry_shader;
    std::_string _pixel_shader;
    std::_vector < JSONShaderResourceInfo > _resource_info;
    std::_vector < JSONConstantBufferInfo > _constant_info;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONShader,
    in_pipeline_state,
    in_vertex_shader,
    in_geometry_shader,
    in_pixel_shader,
    in_resource_info,
    in_constant_info
    );
