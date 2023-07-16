#pragma once
#include "common/json/json_render_target.h"
#include "json/json.hpp"


struct JSONRenderTargetTexture
{
public:
    std::_vector < RenderTargetFormatData > in_target_format_data_array;
    RenderTargetDepthData in_target_depth_data;
    int in_width;
    int in_height;
    bool in_resize_with_screen;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONRenderTargetTexture,
    in_target_format_data_array,
    in_target_depth_data,
    in_width,
    in_height,
    in_resize_with_screen
    );
