#include "common/common_pch.h"

#include "common/json/json_draw_system_enum.h"
#include "common/json/json_input_element_desc.h"
#include "common/json/json_shader_resource.h"

// Typedef struct D3D12_RESOURCE_DESC
// {
// D3D12_RESOURCE_DIMENSION Dimension;
// UINT64 Alignment;
// UINT64 Width;
// UINT Height;
// UINT16 DepthOrArraySize;
// UINT16 MipLevels;
// DXGI_FORMAT Format;
// DXGI_SAMPLE_DESC SampleDesc;
// D3D12_TEXTURE_LAYOUT Layout;
// D3D12_RESOURCE_FLAGS Flags;
// } 	D3D12_RESOURCE_DESC;
void to_json(
    in_nlohmann::json&in_j,
    const D3D12_RESOURCE_DESC&in_p
    )
{
    in_j;
    in_p;
}

void from_json(
    const in_nlohmann::json&in_j,
    D3D12_RESOURCE_DESC&in_p
    )
{
    memset(
        &in_p,
        0,
        sizeof (D3D12_RESOURCE_DESC)
        );
    if (in_j.contains("Dimension"))
    {
        from_json(
            in_j.in_at("Dimension"),
            in_p.Dimension
            );
    }
#if 1
        in_p.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
#else
        if (in_j.contains("Alignment"))
        {
            // D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
            if (in_j.in_at("Alignment") .is_string())
            {
                std::string value;
                // From_json(j.at("Shader4ComponentMapping"), value);
                in_j.in_at("Alignment") .get_to(value);
                if (value == "D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT")
                {
                    in_p.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
                }
                // Other defines?
            }
        }
#endif

    if (in_j.contains("Width"))
    {
        in_j.in_at("Width") .get_to(in_p.Width);
    }
    if (in_j.contains("Height"))
    {
        in_j.in_at("Height") .get_to(in_p.Height);
    }
    if (in_j.contains("DepthOrArraySize"))
    {
        in_j.in_at("DepthOrArraySize") .get_to(in_p.DepthOrArraySize);
    }
    if (in_j.contains("MipLevels"))
    {
        in_j.in_at("MipLevels") .get_to(in_p.MipLevels);
    }
    if (in_j.contains("Format"))
    {
        from_json(
            in_j.in_at("Format"),
            in_p.Format
            );
    }
    if (in_j.contains("SampleDesc"))
    {
        from_json(
            in_j.in_at("SampleDesc"),
            in_p.SampleDesc
            );
    }
    if (in_j.contains("Layout"))
    {
        in_j.in_at("Layout") .get_to(in_p.Layout);
    }
    if (in_j.contains("Flags"))
    {
        in_j.in_at("Flags") .get_to(in_p.Flags);
    }
}

// Typedef struct D3D12_TEX2D_SRV
// {
// UINT MostDetailedMip;
// UINT MipLevels;
// UINT PlaneSlice;
// FLOAT ResourceMinLODClamp;
// }
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_TEX2D_SRV,
    MostDetailedMip,
    MipLevels,
    PlaneSlice,
    ResourceMinLODClamp
    );
// Typedef struct D3D12_SHADER_RESOURCE_VIEW_DESC {
// DXGI_FORMAT         Format;
// D3D12_SRV_DIMENSION ViewDimension;
// UINT                Shader4ComponentMapping;
// Union {
// D3D12_BUFFER_SRV                            Buffer;
// D3D12_TEX1D_SRV                             Texture1D;
// D3D12_TEX1D_ARRAY_SRV                       Texture1DArray;
// D3D12_TEX2D_SRV                             Texture2D;
// D3D12_TEX2D_ARRAY_SRV                       Texture2DArray;
// D3D12_TEX2DMS_SRV                           Texture2DMS;
// D3D12_TEX2DMS_ARRAY_SRV                     Texture2DMSArray;
// D3D12_TEX3D_SRV                             Texture3D;
// D3D12_TEXCUBE_SRV                           TextureCube;
// D3D12_TEXCUBE_ARRAY_SRV                     TextureCubeArray;
// D3D12_RAYTRACING_ACCELERATION_STRUCTURE_SRV RaytracingAccelerationStructure;
// };
// } D3D12_SHADER_RESOURCE_VIEW_DESC;
void to_json(
    in_nlohmann::json&in_j,
    const D3D12_SHADER_RESOURCE_VIEW_DESC&in_p
    )
{
    in_j;
    in_p;
}

void from_json(
    const in_nlohmann::json&in_j,
    D3D12_SHADER_RESOURCE_VIEW_DESC&in_p
    )
{
    memset(
        &in_p,
        0,
        sizeof (D3D12_SHADER_RESOURCE_VIEW_DESC)
        );
    if (in_j.contains("Format"))
    {
        from_json(
            in_j.in_at("Format"),
            in_p.Format
            );
    }
    if (in_j.contains("ViewDimension"))
    {
        from_json(
            in_j.in_at("ViewDimension"),
            in_p.ViewDimension
            );
    }
#if 1
        in_p.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
#else
        if (in_j.contains("Shader4ComponentMapping"))
        {
            if (in_j.in_at("Shader4ComponentMapping") .is_string())
            {
                std::string value;
                // From_json(j.at("Shader4ComponentMapping"), value);
                in_j.in_at("Shader4ComponentMapping") .get_to(value);
                if (value == "D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING")
                {
                    in_p.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                }
                // Other defines?
            }
            else
            {
                // Other encode
            }
        }
#endif

    if (in_j.contains("Texture2D"))
    {
        from_json(
            in_j.in_at("Texture2D"),
            in_p.Texture2D
            );
    }
}

