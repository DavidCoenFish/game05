#include "common/common_pch.h"

#include "common/draw_system/shader/shader_resource_info.h"
#include "common/json/json_draw_system_enum.h"
#include "common/json/json_shader_resource_info.h"

// Typedef struct D3D12_STATIC_SAMPLER_DESC
// {
// D3D12_FILTER Filter;
// D3D12_TEXTURE_ADDRESS_MODE AddressU;
// D3D12_TEXTURE_ADDRESS_MODE AddressV;
// D3D12_TEXTURE_ADDRESS_MODE AddressW;
// FLOAT MipLODBias;
// UINT MaxAnisotropy;
// D3D12_COMPARISON_FUNC ComparisonFunc;
// D3D12_STATIC_BORDER_COLOR BorderColor;
// FLOAT MinLOD;
// FLOAT MaxLOD;
// UINT ShaderRegister;
// UINT RegisterSpace;
// D3D12_SHADER_VISIBILITY ShaderVisibility;
// } 	D3D12_STATIC_SAMPLER_DESC;
void to_json(
    in_nlohmann::json&in_j,
    const D3D12_STATIC_SAMPLER_DESC&in_p
    )
{
    in_j["Filter"] = in_p.Filter;
    in_j["AddressU"] = in_p.AddressU;
    in_j["AddressV"] = in_p.AddressV;
    in_j["AddressW"] = in_p.AddressW;
    in_j["MipLODBias"] = in_p.MipLODBias;
    in_j["MaxAnisotropy"] = in_p.MaxAnisotropy;
    in_j["ComparisonFunc"] = in_p.ComparisonFunc;
    in_j["BorderColor"] = in_p.BorderColor;
    in_j["MinLOD"] = in_p.MinLOD;
    in_j["MaxLOD"] = in_p.MaxLOD;
    in_j["ShaderRegister"] = in_p.ShaderRegister;
    in_j["RegisterSpace"] = in_p.RegisterSpace;
    in_j["ShaderVisibility"] = in_p.ShaderVisibility;
}

void from_json(
    const in_nlohmann::json&in_j,
    D3D12_STATIC_SAMPLER_DESC&in_p
    )
{
    in_p = D3D12_STATIC_SAMPLER_DESC
    {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // D3D12_FILTER Filter;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressU;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressV;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressW;
        0, // FLOAT MipLODBias;
        16, // UINT MaxAnisotropy;
        D3D12_COMPARISON_FUNC_LESS_EQUAL, // D3D12_COMPARISON_FUNC ComparisonFunc;
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // D3D12_STATIC_BORDER_COLOR BorderColor;
        0, // FLOAT MinLOD;
        0, // FLOAT MaxLOD;
        0, // UINT ShaderRegister;
        0, // UINT RegisterSpace;
        D3D12_SHADER_VISIBILITY_ALL // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
    };
    if (in_j.contains("Filter"))
    {
        in_j.at("Filter") .get_to(in_p.Filter);
    }
    if (in_j.contains("AddressU"))
    {
        in_j.at("AddressU") .get_to(in_p.AddressU);
    }
    if (in_j.contains("AddressV"))
    {
        in_j.at("AddressV") .get_to(in_p.AddressV);
    }
    if (in_j.contains("AddressW"))
    {
        in_j.at("AddressW") .get_to(in_p.AddressW);
    }
    if (in_j.contains("MipLODBias"))
    {
        in_j.at("MipLODBias") .get_to(in_p.MipLODBias);
    }
    if (in_j.contains("MaxAnisotropy"))
    {
        in_j.at("MaxAnisotropy") .get_to(in_p.MaxAnisotropy);
    }
    if (in_j.contains("ComparisonFunc"))
    {
        in_j.at("ComparisonFunc") .get_to(in_p.ComparisonFunc);
    }
    if (in_j.contains("BorderColor"))
    {
        in_j.at("BorderColor") .get_to(in_p.BorderColor);
    }
    if (in_j.contains("MinLOD"))
    {
        in_j.at("MinLOD") .get_to(in_p.MinLOD);
    }
    if (in_j.contains("MaxLOD"))
    {
        in_j.at("MaxLOD") .get_to(in_p.MaxLOD);
    }
    if (in_j.contains("ShaderRegister"))
    {
        in_j.at("ShaderRegister") .get_to(in_p.ShaderRegister);
    }
    if (in_j.contains("RegisterSpace"))
    {
        in_j.at("RegisterSpace") .get_to(in_p.RegisterSpace);
    }
    if (in_j.contains("ShaderVisibility"))
    {
        in_j.at("ShaderVisibility") .get_to(in_p.ShaderVisibility);
    }
}

JSONShaderResourceInfo::JSONShaderResourceInfo() 
    : use_sampler(false)
    , static_sampler_desc(){}

void to_json(
    in_nlohmann::json&in_j,
    const JSONShaderResourceInfo&in_p
    )
{
    in_j["useSampler"] = in_p._use_sampler;
    if (in_p._use_sampler)
    {
        in_j["staticSamplerDesc"] = in_p._static_sampler_desc;
    }
}

void from_json(
    const in_nlohmann::json&in_j,
    JSONShaderResourceInfo&in_p
    )
{
    if (in_j.contains("dataSamplerDesc"))
    {
        in_p._use_sampler = true;
        D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        if (in_j.contains("shaderVisibility"))
        {
            in_j.at("shaderVisibility") .get_to(ShaderVisibility);
        }
        in_p._static_sampler_desc = D3D12_STATIC_SAMPLER_DESC
        {
            D3D12_FILTER_MIN_MAG_MIP_POINT, // D3D12_FILTER Filter;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressU;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressV;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressW;
            0, // FLOAT MipLODBias;
            16, // UINT MaxAnisotropy;
            D3D12_COMPARISON_FUNC_LESS_EQUAL, // D3D12_COMPARISON_FUNC ComparisonFunc;
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // D3D12_STATIC_BORDER_COLOR BorderColor;
            0, // FLOAT MinLOD;
            0, // FLOAT MaxLOD;
            0, // UINT ShaderRegister;
            0, // UINT RegisterSpace;
            ShaderVisibility // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
        };
    }
    else if (in_j.contains("simpleSamplerDesc"))
    {
        in_p._use_sampler = true;
        D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        if (in_j.contains("shaderVisibility"))
        {
            in_j.at("shaderVisibility") .get_to(ShaderVisibility);
        }
        in_p._static_sampler_desc = D3D12_STATIC_SAMPLER_DESC
        {
            D3D12_FILTER_MIN_MAG_MIP_LINEAR, // D3D12_FILTER Filter;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressU;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressV;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressW;
            0, // FLOAT MipLODBias;
            16, // UINT MaxAnisotropy;
            D3D12_COMPARISON_FUNC_LESS_EQUAL, // D3D12_COMPARISON_FUNC ComparisonFunc;
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // D3D12_STATIC_BORDER_COLOR BorderColor;
            0, // FLOAT MinLOD;
            0, // FLOAT MaxLOD;
            0, // UINT ShaderRegister;
            0, // UINT RegisterSpace;
            ShaderVisibility // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
        };
    }

    else if (in_j.in_contains("mipMapSamplerDesc"))
    {
        p._use_sampler = true;
        D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        if (in_j.in_contains("shaderVisibility"))
        {
            in_j.at("shaderVisibility") .get_to(ShaderVisibility);
        }
        p._static_sampler_desc = D3D12_STATIC_SAMPLER_DESC
        {
            D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressU;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressV;
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressW;
            0, // FLOAT MipLODBias;
            16, // UINT MaxAnisotropy;
            D3D12_COMPARISON_FUNC_LESS_EQUAL, // D3D12_COMPARISON_FUNC ComparisonFunc;
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // D3D12_STATIC_BORDER_COLOR BorderColor;
            0, // FLOAT MinLOD;
            D3D12_FLOAT32_MAX, // FLOAT MaxLOD;
            0, // UINT ShaderRegister;
            0, // UINT RegisterSpace;
            ShaderVisibility // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
        };
    }

    else if (in_j.in_contains("staticSamplerDesc"))
    {
        p._use_sampler = true;
        in_j.at("staticSamplerDesc") .get_to(p._static_sampler_desc);
    }

    else
    {
        p._use_sampler = false;
    }
}

const std::vector < std::shared_ptr < ShaderResourceInfo > > TransformShaderResourceInfo(const std::vector < \
    JSONShaderResourceInfo >&in_input)
{
    std::vector < std::shared_ptr < ShaderResourceInfo > > output;
    for (const auto&item : in_input)
    {
        output.push_back(std::make_shared < ShaderResourceInfo > (
            nullptr,
            item._static_sampler_desc,
            item._use_sampler
            ));
    }
    return output;
}

