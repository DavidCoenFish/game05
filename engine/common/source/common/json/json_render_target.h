#pragma once
#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"
#include "common/json/json_vector.h"
#include "json/json.hpp"


class DrawSystem;
class CustomCommandList;
class Shader;
class FileCache;

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// RenderTargetFormatData,
// Format,
// ClearOnSet,
// ClearColor
// );
void to_json(
    nlohmann::json& in_j,
    const RenderTargetFormatData& in_p
    );
void from_json(
    const nlohmann::json& in_j,
    RenderTargetFormatData& in_p
    );
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// RenderTargetDepthData,
// Format,
// ClearDepthOnSet,
// ClearDepth,
// ClearStencilOnSet,
// ClearStencil,
// ShaderResource
// );
void to_json(
    nlohmann::json& in_j,
    const RenderTargetDepthData& in_p
    );
void from_json(
    const nlohmann::json& in_j,
    RenderTargetDepthData& in_p
    );
struct JSONRenderTarget
{
public:
    std::vector < RenderTargetFormatData > _target_format_data_array;
    RenderTargetDepthData _target_depth_data;
    int _width;
    int _height;
    bool _resize_with_screen;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONRenderTarget,
    _target_format_data_array,
    _target_depth_data,
    _width,
    _height,
    _resize_with_screen
    );
