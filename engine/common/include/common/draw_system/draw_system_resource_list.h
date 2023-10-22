#pragma once
class IResource;

/// Have been having issues with [render targets, geometry, constant buffer] being destoyed before command list finishes
class DrawSystemResourceList
{
public:
    static std::shared_ptr<DrawSystemResourceList> Factory(
        ID3D12Device& in_device
        );

    /// move fence creation internal, fence created against value of zero, is set to one once finished
    DrawSystemResourceList(
        Microsoft::WRL::ComPtr<ID3D12Fence>& in_fence
        );
    /// dtor will decrement refereces to added IResource
    ~DrawSystemResourceList();

    /// Add a reference to resources that needs to be kept alive till the fence is passed in the command list
    void AddResource(
        const std::shared_ptr<IResource>& in_resource
        );

    /// Add to the command list a fence so we know when it is done, assert if any more calls to AddResource are made
    void MarkFinished(
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& in_command_queue
        );

    /// Return true if the relevant command list has finished
    const bool GetFinished() const;

private:
    std::vector<std::shared_ptr<IResource>> _resource_array;
    Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
    bool _command_list_marked_finished;

};
