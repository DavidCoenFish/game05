#pragma once
#include "common/draw_system/i_resource.h"

class HeapWrapperItem;

class UnorderedAccess : public IResource
{
public:
    UnorderedAccess(
        DrawSystem* const in_draw_system,
        const std::shared_ptr < HeapWrapperItem >&in_heap_wrapper_item,
        const D3D12_RESOURCE_DESC&in_desc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC&in_unordered_access_view_desc
        );
    std::shared_ptr < HeapWrapperItem > GetHeapWrapperItem() const;

private:
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) override;

private:
    Microsoft::WRL::ComPtr < ID3D12Resource > _resource;
    std::shared_ptr < HeapWrapperItem > _heap_wrapper_item;
    D3D12_RESOURCE_DESC _desc;
    D3D12_UNORDERED_ACCESS_VIEW_DESC _unordered_access_view_desc;
};
