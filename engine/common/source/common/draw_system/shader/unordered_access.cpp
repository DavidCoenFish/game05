#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/unordered_access.h"
#include "common/draw_system/shader/shader_resource.h"

UnorderedAccess::UnorderedAccess(
    DrawSystem* const in_draw_system,
    const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item,
    const std::shared_ptr<HeapWrapperItem>& in_shader_view_heap_wrapper_or_null,
    const D3D12_RESOURCE_DESC& in_desc,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC& in_unordered_access_view_desc,
    const std::vector<uint8_t>& in_data
)
    : IResource(in_draw_system)
    , _heap_wrapper_item(in_heap_wrapper_item)
    , _shader_view_heap_wrapper_item(in_shader_view_heap_wrapper_or_null)
    , _desc(in_desc)
    , _unordered_access_view_desc(in_unordered_access_view_desc)
    , _data(in_data)
{
    return;
}

void UnorderedAccess::OnDeviceLost()
{
    _resource.Reset();
}

std::shared_ptr<HeapWrapperItem> UnorderedAccess::GetHeapWrapperItem() const
{
    return _heap_wrapper_item;
}

std::shared_ptr<HeapWrapperItem> UnorderedAccess::GetShaderViewHeapWrapperItem() const
{
    return _shader_view_heap_wrapper_item;
}

void UnorderedAccess::OnDeviceRestored(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device
    )
{
    CD3DX12_HEAP_PROPERTIES heap_default(D3D12_HEAP_TYPE_DEFAULT);
    DX::ThrowIfFailed(in_device->CreateCommittedResource(
        &heap_default,
        D3D12_HEAP_FLAG_NONE,
        &_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf())
        ));
    _resource->SetName(L"UnorderedAccess resource");

    ShaderResource::UploadResource(
        _draw_system,
        in_command_list,
        _resource,
        _desc,
        _data.size(),
        _data.size() ? _data.data() : nullptr
        );

    in_device->CreateUnorderedAccessView(
        _resource.Get(),
        nullptr,
        &_unordered_access_view_desc,
        _heap_wrapper_item->GetCPUHandleFrame(0)
        );
    return;
}

