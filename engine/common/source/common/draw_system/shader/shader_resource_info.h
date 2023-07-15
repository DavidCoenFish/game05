#pragma once
#include "common/draw_system/d3dx12.h"

class HeapWrapperItem;

struct ShaderResourceInfo
{
public:
    static std::_shared_ptr < ShaderResourceInfo > FactorySampler(
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
        const D3D12_SHADER_VISIBILITY in_visiblity,
        // D3D12_SHADER_VISIBILITY_PIXEL
        const bool in_mip_map = false
        );
    static std::shared_ptr < ShaderResourceInfo > FactoryDataSampler(
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
        const D3D12_SHADER_VISIBILITY in_visiblity
        );
    static std::shared_ptr < ShaderResourceInfo > FactoryNoSampler(
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle,
        const D3D12_SHADER_VISIBILITY in_visiblity
        );
    explicit ShaderResourceInfo(
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle = nullptr,
        const D3D12_STATIC_SAMPLER_DESC&in_static_sampler_desc = CD3DX12_STATIC_SAMPLER_DESC(),
        const bool in_use_sampler = true
        );
    void Activate(
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_root_param_index
        );
    // Void ActivateSampler(
    // ID3D12GraphicsCommandList* const pCommandList,
    // Const int rootParamIndex
    // );
    void SetShaderResourceViewHandle(const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_handle);
    const bool GetUseSampler() const
    {
        return use_sampler;
    }

    const D3D12_STATIC_SAMPLER_DESC&GetStaticSamplerDesc() const
    {
        return static_sampler_desc;
    }

    const D3D12_SHADER_VISIBILITY GetVisiblity() const;

private:
    bool _use_sampler;
    std::_shared_ptr < HeapWrapperItem > _shader_resource_view_handle;
    // Std::shared_ptr< HeapWrapperItem > m_pSamplerHandle;
    D3D12_STATIC_SAMPLER_DESC _static_sampler_desc;
};
