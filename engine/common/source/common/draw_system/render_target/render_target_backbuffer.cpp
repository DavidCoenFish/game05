#include "common/common_pch.h"

#include "common/draw_system/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/render_target/render_target_backbuffer.h"

RenderTargetBackBuffer::RenderTargetBackBuffer(
    DrawSystem* const in_draw_system,
    ID3D12Device* const in_device,
    const int in_buffer_index,
    const RenderTargetFormatData&in_target_format_data,
    const RenderTargetDepthData&in_target_depth_data,
    IDXGISwapChain* const in_swap_chain,
    const int in_width,
    const int in_height
    ) 
    : IRenderTarget()
    , buffer_index(in_buffer_index)
    , target_format_data(in_target_format_data)
    , target_clear_value(in_target_format_data.MakeClearValue())
    , target_depth_data(in_target_depth_data)
    , depth_clear_value(in_target_depth_data.MakeClearValue())
    , current_state(D3D12_RESOURCE_STATE_COMMON)
    , screen_viewport{ 0.0f
    , 0.0f
    , static_cast < float > (in_width)
    , static_cast < float > (in_height)
    , D3D12_MIN_DEPTH
    , D3D12_MAX_DEPTH}
    , scissor_rect{ 0
    , 0
    , in_width
    , in_height}
    , back_buffer_width(in_width)
    , back_buffer_height(in_height)
{
    render_target_descriptor = in_draw_system->MakeHeapWrapperRenderTargetView();
    if (DXGI_FORMAT_UNKNOWN != target_depth_data._format)
    {
        depth_stencil_view_descriptor = in_draw_system->MakeHeapWrapperDepthStencilView();
    }
    // Back buffer render target view

    {
        DX::ThrowIfFailed(in_swap_chain->GetBuffer(
            buffer_index,
            IID_PPV_ARGS(render_target_resource.GetAddressOf())
            ));
        wchar_t name[32] = {};
        swprintf_s(
            name,
            L"Render target %u",
            buffer_index
            );
        render_target_resource->SetName(name);
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        rtv_desc.Format = target_format_data._format;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        auto rtv_descriptor = render_target_descriptor->GetCPUHandle();
        in_device->CreateRenderTargetView(
            render_target_resource.Get(),
            &rtv_desc,
            rtv_descriptor
            );
    }
    // Depth buffer
    if (DXGI_FORMAT_UNKNOWN != target_depth_data._format)
    {
        // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
        // On this surface.
        CD3DX12_HEAP_PROPERTIES depth_heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC depth_stencil_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            target_depth_data._format,
            in_width,
            in_height,
            1,
            // This depth stencil view has only one texture.
            0,
            // Use a single mipmap level.
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
            );
        depth_stencil_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        DX::ThrowIfFailed(in_device->CreateCommittedResource(
            &depth_heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &depth_stencil_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depth_clear_value,
            IID_PPV_ARGS(depth_resource.ReleaseAndGetAddressOf())
            ));
        wchar_t name[32] = {};
        swprintf_s(
            name,
            L"Depth stencil %u",
            buffer_index
            );
        depth_resource->SetName(name);
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
        dsv_desc.Format = target_depth_data._format;
        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        auto handle = depth_stencil_view_descriptor->GetCPUHandle();
        in_device->CreateDepthStencilView(
            depth_resource.Get(),
            &dsv_desc,
            handle
            );
    }
    return;
}

void RenderTargetBackBuffer::StartRender(ID3D12GraphicsCommandList* const in_command_list)
{
    if (current_state != D3D12_RESOURCE_STATE_RENDER_TARGET)
    {
        // Transition the render target into the correct state to allow for drawing into it.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            in_render_target_resource.Get(),
            current_state,
            D3D12_RESOURCE_STATE_RENDER_TARGET
            );
        in_command_list->ResourceBarrier(
            1,
            &barrier
            );
        current_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    in_command_list->RSSetViewports(
        1,
        &in_screen_viewport
        );
    in_command_list->RSSetScissorRects(
        1,
        &in_scissor_rect
        );
    auto render_target_descriptor_handle = render_target_descriptor->GetCPUHandle();
    if (depth_stencil_view_descriptor)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE dsv_descriptor = depth_stencil_view_descriptor->GetCPUHandle();
        in_command_list->OMSetRenderTargets(
            1,
            &render_target_descriptor_handle,
            FALSE,
            &dsv_descriptor
            );
    }
    else
    {
        in_command_list->OMSetRenderTargets(
            1,
            &render_target_descriptor_handle,
            FALSE,
            nullptr
            );
    }

    {
        PIXBeginEvent(
            in_command_list,
            PIX_COLOR_DEFAULT,
            L"RenderTargetBackBuffer::Clear"
            );
        if ((nullptr != render_target_descriptor) && target_format_data._clear_on_set)
        {
            auto descriptor = render_target_descriptor->GetCPUHandle();
            in_command_list->ClearRenderTargetView(
                descriptor,
                in_target_clear_value.Color,
                0,
                nullptr
                );
        }
        if ((nullptr != depth_stencil_view_descriptor) && (target_depth_data._clear_depth_on_set || target_depth_data.
            _clear_stencil_on_set))
        {
            const D3D12_CLEAR_FLAGS flag = (D3D12_CLEAR_FLAGS)((target_depth_data._clear_depth_on_set ? 
                D3D12_CLEAR_FLAG_DEPTH : 0) | (target_depth_data._clear_stencil_on_set ? D3D12_CLEAR_FLAG_STENCIL : 0));
            auto descriptor = depth_stencil_view_descriptor->GetCPUHandle();
            in_command_list->ClearDepthStencilView(
                descriptor,
                flag,
                in_depth_clear_value.DepthStencil.Depth,
                in_depth_clear_value.DepthStencil.Stencil,
                0,
                nullptr
                );
        }
        PIXEndEvent(in_command_list);
    }
}

void RenderTargetBackBuffer::EndRender(ID3D12GraphicsCommandList* const in_command_list)
{
    if (current_state != D3D12_RESOURCE_STATE_PRESENT)
    {
        // Transition the render target to the state that allows it to be presented to the display.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            in_render_target_resource.Get(),
            current_state,
            D3D12_RESOURCE_STATE_PRESENT
            );
        in_command_list->ResourceBarrier(
            1,
            &barrier
            );
        current_state = D3D12_RESOURCE_STATE_PRESENT;
    }
}

void RenderTargetBackBuffer::GetFormatData(
    DXGI_FORMAT&in_depth_format,
    int&in_render_target_view_format_count,
    const DXGI_FORMAT*&in_render_target_view_format
    ) const
{
    in_depth_format = target_depth_data._format;
    in_render_target_view_format_count = 1;
    in_render_target_view_format =&target_format_data._format;
}

const int RenderTargetBackBuffer::GetWidth() const
{
    return back_buffer_width;
}

const int RenderTargetBackBuffer::GetHeight() const
{
    return back_buffer_height;
}

