#include "common/common_pch.h"

#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/unordered_access_info.h"

UnorderedAccessInfo::UnorderedAccessInfo(
    const std::shared_ptr < HeapWrapperItem >&in_unordered_access_view_handle,
    const D3D12_SHADER_VISIBILITY in_visiblity
    ) 
    : _unordered_access_view_handle(in_unordered_access_view_handle)
    , _visiblity(in_visiblity)
{
    return;
}

void UnorderedAccessInfo::SetUnorderedAccessViewHandle(const std::shared_ptr < HeapWrapperItem >&\
    in_unordered_access_view_handle)
{
    _unordered_access_view_handle = in_unordered_access_view_handle;
    return;
}

void UnorderedAccessInfo::Activate(
    ID3D12GraphicsCommandList* const in_command_list,
    const int in_root_param_index
    )
{
    if (_unordered_access_view_handle)
    {
        auto heap = _unordered_access_view_handle->GetHeap();
        in_command_list->SetDescriptorHeaps(
            1,
            &heap
            );
        in_command_list->SetComputeRootDescriptorTable(
            in_root_param_index,
            _unordered_access_view_handle->GetGPUHandle()
            );
    }
    return;
}

