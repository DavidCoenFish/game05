#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/unordered_access.h"

UnorderedAccess::UnorderedAccess(
    DrawSystem* const in_draw_system,
    const std::shared_ptr < HeapWrapperItem >&in_heap_wrapper_item,
    const D3D12_RESOURCE_DESC&in_desc,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC&in_unordered_access_view_desc
    ) 
    : IResource(in_draw_system)
    , _heap_wrapper_item(in_heap_wrapper_item)
    , _desc(in_desc)
    , _unordered_access_view_desc(in_unordered_access_view_desc)
{
    return;
}

void UnorderedAccess::OnDeviceLost()
{
    _resource.Reset();
}

std::shared_ptr < HeapWrapperItem > UnorderedAccess::GetHeapWrapperItem() const
{
    return _heap_wrapper_item;
}

void UnorderedAccess::OnDeviceRestored(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const in_device
    )
{
    CD3DX12_HEAP_PROPERTIES heap_default(D3D12_HEAP_TYPE_DEFAULT);
    in_device->CreateCommittedResource(
        &heap_default,
        D3D12_HEAP_FLAG_NONE,
        &_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf())
        );
    in_device->CreateUnorderedAccessView(
        _resource.Get(),
        nullptr,
        &_unordered_access_view_desc,
        _heap_wrapper_item->GetCPUHandleFrame(0)
        );
    return;
}

