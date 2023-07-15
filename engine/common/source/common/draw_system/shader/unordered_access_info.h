#pragma once
#include "common/draw_system/d3dx12.h"

class HeapWrapperItem;

struct UnorderedAccessInfo
{
public:
    explicit UnorderedAccessInfo(
        const std::shared_ptr < HeapWrapperItem >&in_unordered_access_view_handle = nullptr,
        const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
        );
    void SetUnorderedAccessViewHandle(const std::shared_ptr < HeapWrapperItem >&in_unordered_access_view_handle);
    const D3D12_SHADER_VISIBILITY GetVisiblity() const
    {
        return visiblity;
    }

    void Activate(
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_root_param_index
        );

private:
    std::_shared_ptr < HeapWrapperItem > _unordered_access_view_handle;
    D3D12_SHADER_VISIBILITY _visiblity;
};
