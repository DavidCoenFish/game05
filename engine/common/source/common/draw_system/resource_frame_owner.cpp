#include "common/common_pch.h"

#include "common/draw_system/resource_frame_owner.h"

/*
*/

ResourceFrameOwner::ResourceFrameOwner()
    : _fence_value(0)
{
}

ResourceFrameOwner::~ResourceFrameOwner()
{
}

void ResourceFrameOwner::AddResource(
    const std::shared_ptr<IResource>& in_resource
    )
{
}

void ResourceFrameOwner::EndFrame(
    ID3D12CommandQueue* const in_command_queue
    )
{
    DX::ThrowIfFailed(in_command_queue->Signal(
        _fence.Get(),
        _fence_value
        ));

}
