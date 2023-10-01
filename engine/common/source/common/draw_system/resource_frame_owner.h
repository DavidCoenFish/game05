#pragma once

class IResource;

///  We want to extend the lifespan of render resource till the consumption of a command list
class ResourceFrameOwner
{
public:
    ResourceFrameOwner();
    ~ResourceFrameOwner();

    void AddResource(
        const std::shared_ptr<IResource>& in_resource
        );
    void EndFrame(
        ID3D12CommandQueue* const in_command_queue
        );

private:
    struct FrameResources
    {
        UINT64 _fence_value;
        std::vector<std::shared_ptr<IResource>> _resource_array;
    };

    std::vector<std::shared_ptr<FrameResources>> _frame_resource_array;

    UINT64 _fence_value;
    Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
    Microsoft::WRL::Wrappers::Event _fence_event;

};
