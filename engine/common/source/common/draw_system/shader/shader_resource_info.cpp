#include "common/common_pch.h"

#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/shader_resource_info.h"

std::shared_ptr < ShaderResourceInfo > ShaderResourceInfo::FactorySampler(
    const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
    const D3D12_SHADER_VISIBILITY in_visiblity,
    const bool in_mip_map
    )
{
    D3D12_STATIC_SAMPLER_DESC static_sampler_desc
    {
        in_mip_map ? D3D12_FILTER_ANISOTROPIC : D3D12_FILTER_MIN_MAG_MIP_LINEAR, // D3D12_FILTER Filter;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressU;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressV;
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // D3D12_TEXTURE_ADDRESS_MODE AddressW;
        0, // FLOAT MipLODBias;
        16, // UINT MaxAnisotropy;
        D3D12_COMPARISON_FUNC_LESS_EQUAL, // D3D12_COMPARISON_FUNC ComparisonFunc;
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // D3D12_STATIC_BORDER_COLOR BorderColor;
        0, // FLOAT MinLOD;
        in_mip_map ? D3D12_FLOAT32_MAX : 0, // FLOAT MaxLOD;
        0, // UINT ShaderRegister;
        0, // UINT RegisterSpace;
        in_visiblity // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
        }
        ; return std::make_shared < ShaderResourceInfo > (
            in_shader_resource_view_handle,
            static_sampler_desc
            );
    }

    std::shared_ptr < ShaderResourceInfo > ShaderResourceInfo::FactoryDataSampler(
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
        const D3D12_SHADER_VISIBILITY in_visiblity
        )
    {
        D3D12_STATIC_SAMPLER_DESC static_sampler_desc
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
            in_visiblity // D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
            }
            ; return std::make_shared < ShaderResourceInfo > (
                in_shader_resource_view_handle,
                static_sampler_desc
                );
        }

        std::shared_ptr < ShaderResourceInfo > ShaderResourceInfo::FactoryNoSampler(
            const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
            const D3D12_SHADER_VISIBILITY in_visiblity
            )
        {
            D3D12_STATIC_SAMPLER_DESC static_sampler_desc {};
            static_sampler_desc.ShaderVisibility = in_visiblity;
            return std::make_shared < ShaderResourceInfo > (
                in_shader_resource_view_handle,
                static_sampler_desc,
                false
                );
        }

        ShaderResourceInfo::ShaderResourceInfo(
            const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
            const D3D12_STATIC_SAMPLER_DESC&in_static_sampler_desc,
            const bool in_use_sampler
            ) 
            : _shader_resource_view_handle(in_shader_resource_view_handle)
            , _static_sampler_desc(in_static_sampler_desc)
            , _use_sampler(in_use_sampler)
        {
            return;
        }

        void ShaderResourceInfo::Activate(
            ID3D12GraphicsCommandList* const in_command_list,
            const int in_root_param_index
            )
        {
            if (_shader_resource_view_handle)
            {
                auto heap = _shader_resource_view_handle->GetHeap();
                in_command_list->SetDescriptorHeaps(
                    1,
                    &heap
                    );
                in_command_list->SetGraphicsRootDescriptorTable(
                    in_root_param_index,
                    _shader_resource_view_handle->GetGPUHandle()
                    );
            }
            return;
        }

        void ShaderResourceInfo::SetShaderResourceViewHandle(const std::shared_ptr < HeapWrapperItem >&\
            in_shader_resource_view_handle)
        {
            _shader_resource_view_handle = in_shader_resource_view_handle;
            return;
        }

        const D3D12_SHADER_VISIBILITY ShaderResourceInfo::GetVisiblity() const
        {
            return _static_sampler_desc.ShaderVisibility;
        }

