#pragma once
#include "common/direct_xtk12/d3dx12.h"

class HeapWrapperItem;

struct UnorderedAccessInfo
{
public:
    static std::shared_ptr<UnorderedAccessInfo> Factory(
        const std::shared_ptr<HeapWrapperItem>& in_unordered_access_view_handle = nullptr,
        const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
        );

    explicit UnorderedAccessInfo(
        const std::shared_ptr<HeapWrapperItem>& in_unordered_access_view_handle = nullptr,
        const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
        );
    void SetUnorderedAccessViewHandle(const std::shared_ptr<HeapWrapperItem>& in_unordered_access_view_handle);
    const D3D12_SHADER_VISIBILITY GetVisiblity() const
    {
        return _visiblity;
    }

    void Activate(
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_root_param_index
        );

private:
    std::shared_ptr<HeapWrapperItem> _unordered_access_view_handle;
    D3D12_SHADER_VISIBILITY _visiblity;
};
