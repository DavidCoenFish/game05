#pragma once
#include "common/json/json_render_target.h"
#include "json/json.hpp"


class DrawSystem;
class CustomCommandList;
class Shader;
class FileCache;

class JSONDrawSystem
{
public:
    JSONDrawSystem() 
        : _back_buffer_count(2)
        , _d3d_feature_level(D3D_FEATURE_LEVEL_11_0)
        , _options(0)
    {
        // Nop
    }

public:
    unsigned int _back_buffer_count;
    D3D_FEATURE_LEVEL _d3d_feature_level;
    unsigned int _options;
    RenderTargetFormatData _target_format_data;
    RenderTargetDepthData _target_depth_data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONDrawSystem,
    _back_buffer_count,
    _d3d_feature_level,
    _options,
    _target_format_data,
    _target_depth_data
    );
