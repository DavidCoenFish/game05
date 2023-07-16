#pragma once
#include "json/json.hpp"

struct ShaderResourceInfo;

void to_json(
    in_nlohmann::json&in_j,
    const D3D12_STATIC_SAMPLER_DESC&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    D3D12_STATIC_SAMPLER_DESC&in_p
    );
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// D3D12_STATIC_SAMPLER_DESC,
// Filter,
// AddressU,
// AddressV,
// AddressW,
// MipLODBias,
// MaxAnisotropy,
// ComparisonFunc,
// BorderColor,
// MinLOD,
// MaxLOD,
// ShaderRegister,
// RegisterSpace,
// ShaderVisibility
// );
struct JSONShaderResourceInfo
{
public:
    JSONShaderResourceInfo();

public:
    D3D12_STATIC_SAMPLER_DESC _static_sampler_desc;
    bool _use_sampler;
};
void to_json(
    in_nlohmann::json&in_j,
    const JSONShaderResourceInfo&in_p
    );
void from_json(
    const in_nlohmann::json&in_j,
    JSONShaderResourceInfo&in_p
    );
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// JSONShaderResourceInfo,
// StaticSamplerDesc
// );
const std::vector < std::shared_ptr < ShaderResourceInfo > > TransformShaderResourceInfo(const std::vector < \
    JSONShaderResourceInfo >&in_input);
