#include "common/common_pch.h"

#include "common/draw_system/d3dx12.h"
#include "common/draw_system/frame_command_list.h"

CustomCommandList::CustomCommandList(
    ID3D12Device* const in_device,
    ID3D12CommandQueue* const in_command_queue,
    ID3D12GraphicsCommandList* const in_command_list
    ) 
    : device(in_device)
    , command_queue(in_command_queue)
    , command_list(in_command_list)
{
    return;
}

CustomCommandList::~CustomCommandList()
{
    // Send the command list off to the GPU for processing.
    if (command_list)
    {
        DX::ThrowIfFailed(command_list->Close());
    }
    if (command_queue)
    {
        command_queue->ExecuteCommandLists(
            1,
            CommandListCast(&command_list)
            );
    }
    return;
}

void CustomCommandList::UpdateSubresourcesMethod(
    ID3D12Resource* in_destination_resource,
    ID3D12Resource* in_intermediate,
    UINT64 IntermediateOffset,
    UINT FirstSubresource,
    UINT NumSubresources,
    D3D12_SUBRESOURCE_DATA* in_src_data,
    D3D12_BOX* in_dest_box_or_null
    )
{
    UpdateSubresources(
        in_command_list,
        in_destination_resource,
        in_intermediate,
        IntermediateOffset,
        FirstSubresource,
        NumSubresources,
        in_src_data
        );
    in_dest_box_or_null;
    // UpdateSubresources(
    // M_pCommandList,
    // PDestinationResource,
    // PIntermediate,
    // _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
    // _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources,
    // UINT64 RequiredSize,
    // _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
    // _In_reads_(NumSubresources) const UINT* pNumRows,
    // _In_reads_(NumSubresources) const UINT64* pRowSizesInBytes,
    // _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData);
    // UpdateSubresources(
    // M_pCommandList,
    // PDestinationResource,
    // PIntermediate,
    // IntermediateOffset,
    // FirstSubresource,
    // NumSubresources,
    // PSrcData,
    // PDestBoxOrNull
    // );
    return;
}

void CustomCommandList::ResourceBarrier(
    ID3D12Resource* const in_resource,
    D3D12_RESOURCE_STATES in_state_before,
    D3D12_RESOURCE_STATES in_state_after
    )
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        in_resource,
        in_state_before,
        in_state_after
        );
    command_list->ResourceBarrier(
        1,
        &barrier
        );
}

