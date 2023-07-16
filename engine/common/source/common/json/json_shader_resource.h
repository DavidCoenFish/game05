#pragma once
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/json/json_constant_buffer_info.h"
#include "common/json/json_draw_system_enum.h"
#include "json/json.hpp"


// Typedef struct DXGI_SAMPLE_DESC
// {
// UINT Count;
// UINT Quality;
// } DXGI_SAMPLE_DESC;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    DXGI_SAMPLE_DESC,
    Count,
    Quality
    );
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// D3D12_RESOURCE_DESC,
// Dimension,
// Alignment,
// Width,
// Height,
// DepthOrArraySize,
// MipLevels,
// Format,
// SampleDesc,
// Layout,
// Flags
// );
void to_json(
    in_nlohmann::json&in_j,
    const D3D12_RESOURCE_DESC&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    D3D12_RESOURCE_DESC&in_p
    );
// DXGI_FORMAT         Format;
// D3D12_SRV_DIMENSION ViewDimension;
// UINT                Shader4ComponentMapping;
// Union D3D12_TEX2D_SRV Texture2D
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// D3D12_SHADER_RESOURCE_VIEW_DESC,
// Format,
// ViewDimension,
// Shader4ComponentMapping
// );
// Void from_json(const nlohmann::json& j, D3D12_SHADER_RESOURCE_VIEW_DESC& p);
// Struct JSONShaderResourceViewDesc
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// D3D12_SHADER_RESOURCE_VIEW_DESC,
// Format,
// ViewDimension,
// Shader4ComponentMapping,
// //Buffer,
// //Texture1D,
// //Texture1DArray,
// Texture2D
// //Texture2DArray,
// //Texture2DMS,
// //Texture2DMSArray,
// //Texture3D,
// //TextureCube,
// //TextureCubeArray,
// //RaytracingAccelerationStructure
// );
void to_json(
    in_nlohmann::json&in_j,
    const D3D12_SHADER_RESOURCE_VIEW_DESC&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    D3D12_SHADER_RESOURCE_VIEW_DESC&in_p
    );
struct JSONShaderResource
{
public:
    D3D12_RESOURCE_DESC _desc;
    D3D12_SHADER_RESOURCE_VIEW_DESC _shader_resource_view_desc;
    std::_vector < uint8_t > _data;
};
// NLOHMANN_DEFINE_TYPE_INTRUSIVE
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONShaderResource,
    in_desc,
    in_shader_resource_view_desc,
    in_data
    );
