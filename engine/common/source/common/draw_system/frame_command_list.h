#pragma once
class i_render_target;
class Geometry;
class Shader;
class ShaderTexture2D;

class CustomCommandList
{
public:
    CustomCommandList(
        ID3D12Device* const in_device,
        ID3D12CommandQueue* const in_command_queue,
        ID3D12GraphicsCommandList* const in_command_list
        );
    ~CustomCommandList();
    void UpdateSubresourcesMethod(
        ID3D12Resource* in_destination_resource,
        ID3D12Resource* in_intermediate,
        UINT64 IntermediateOffset,
        UINT FirstSubresource,
        UINT NumSubresources,
        D3D12_SUBRESOURCE_DATA* in_src_data,
        D3D12_BOX* in_dest_box_or_null = nullptr
        );
    void ResourceBarrier(
        ID3D12Resource* const in_resource,
        D3D12_RESOURCE_STATES in_state_before,
        D3D12_RESOURCE_STATES in_state_after
        );

private:
    ID3D12Device* _device;
    ID3D12CommandQueue* _command_queue;
    ID3D12GraphicsCommandList* _command_list;
};
