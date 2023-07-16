#pragma once
#include "common/direct_xtk12/d3dx12.h"

struct ShaderPipelineStateData
{
public:
    static ShaderPipelineStateData FactoryComputeShader();
    explicit ShaderPipelineStateData(
        const std::vector < D3D12_INPUT_ELEMENT_DESC >&in_input_element_desc_array = std::vector < \
            D3D12_INPUT_ELEMENT_DESC > (),
        const D3D12_PRIMITIVE_TOPOLOGY_TYPE in_primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
        const DXGI_FORMAT in_depth_stencil_view_format = DXGI_FORMAT_UNKNOWN,
        const std::vector < DXGI_FORMAT >&in_render_target_format = std::vector < DXGI_FORMAT > (),
        const D3D12_BLEND_DESC&in_blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
        // D3D12_DEFAULT),
        const D3D12_RASTERIZER_DESC&in_rasterizer_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
        // D3D12_DEFAULT),
        const D3D12_DEPTH_STENCIL_DESC&in_depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(),
        // D3D12_DEFAULT)
        const bool in_compute_shader = false
        );

public:
    std::vector < D3D12_INPUT_ELEMENT_DESC > _input_element_desc_array;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE _primitive_topology_type;
    DXGI_FORMAT _depth_stencil_view_format;
    std::vector < DXGI_FORMAT > _render_target_format;
    D3D12_BLEND_DESC _blend_state;
    D3D12_RASTERIZER_DESC _rasterizer_state;
    D3D12_DEPTH_STENCIL_DESC _depth_stencil_state;
    bool _compute_shader;
};
