#include "common/common_pch.h"

#include "common/draw_system/draw_system_resource_list.h"

std::shared_ptr<DrawSystemResourceList> DrawSystemResourceList::Factory(
    ID3D12Device& in_device
    )
{
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    // Create a fence for tracking GPU execution progress.
    DX::ThrowIfFailed(in_device.CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())
        ));
    DSC_ASSERT(nullptr != fence, "failed to create fence");
    fence->SetName(L"Fence");

    return std::make_shared<DrawSystemResourceList>(fence);
}

DrawSystemResourceList::DrawSystemResourceList(
    Microsoft::WRL::ComPtr<ID3D12Fence>& in_fence
    )
    : _fence(in_fence)
    , _command_list_marked_finished(false)
{
    // Nop
}

DrawSystemResourceList::~DrawSystemResourceList()
{
    // Nop
}

void DrawSystemResourceList::AddResource(
    const std::shared_ptr<IResource>& in_resource
    )
{
    DSC_ASSERT(false == _command_list_marked_finished, "once a command list has been marked finished, do not add more resources");
    if (nullptr != in_resource)
    {
        _resource_array.push_back(in_resource);
    }
    return;
}

void DrawSystemResourceList::MarkFinished(
    const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& in_command_queue
    )
{
    in_command_queue->Signal(
        _fence.Get(),
        1
        );
    _command_list_marked_finished = true;
    return;
}

const bool DrawSystemResourceList::GetFinished() const
{
    return (0 < _fence->GetCompletedValue());
}

