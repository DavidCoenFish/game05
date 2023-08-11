#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
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
    const VectorInt2& in_size
    ) 
    : IRenderTarget()
    , _buffer_index(in_buffer_index)
    , _target_format_data(in_target_format_data)
    , _target_clear_value(in_target_format_data.MakeClearValue())
    , _target_depth_data(in_target_depth_data)
    , _depth_clear_value(in_target_depth_data.MakeClearValue())
    , _current_state(D3D12_RESOURCE_STATE_COMMON)
    , _screen_viewport{ 
        0.0f 
        , 0.0f
        , static_cast<float>(in_size.GetX())
        , static_cast<float>(in_size.GetY())
        , D3D12_MIN_DEPTH, D3D12_MAX_DEPTH}
    , _scissor_rect{ 0, 0, in_size.GetX(), in_size.GetY() }
    , _back_buffer_size(in_size)
{
    _render_target_descriptor = in_draw_system->MakeHeapWrapperRenderTargetView();
    if (DXGI_FORMAT_UNKNOWN != _target_depth_data._format)
    {
        _depth_stencil_view_descriptor = in_draw_system->MakeHeapWrapperDepthStencilView();
    }
    // Back buffer render target view

    {
        DX::ThrowIfFailed(in_swap_chain->GetBuffer(
            _buffer_index,
            IID_PPV_ARGS(_render_target_resource.GetAddressOf())
            ));
        wchar_t name[32] = {};
        swprintf_s(
            name,
            L"Render target %u",
            _buffer_index
            );
        _render_target_resource->SetName(name);
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        rtv_desc.Format = _target_format_data._format;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        auto rtv_descriptor = _render_target_descriptor->GetCPUHandle();
        in_device->CreateRenderTargetView(
            _render_target_resource.Get(),
            &rtv_desc,
            rtv_descriptor
            );
    }
    // Depth buffer
    if (DXGI_FORMAT_UNKNOWN != _target_depth_data._format)
    {
        // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
        // On this surface.
        CD3DX12_HEAP_PROPERTIES depth_heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC depth_stencil_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            _target_depth_data._format,
            in_size.GetX(),
            in_size.GetY(),
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
            &_depth_clear_value,
            IID_PPV_ARGS(_depth_resource.ReleaseAndGetAddressOf())
            ));
        wchar_t name[32] = {};
        swprintf_s(
            name,
            L"Depth stencil %u",
            _buffer_index
            );
        _depth_resource->SetName(name);
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
        dsv_desc.Format = _target_depth_data._format;
        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        auto handle = _depth_stencil_view_descriptor->GetCPUHandle();
        in_device->CreateDepthStencilView(
            _depth_resource.Get(),
            &dsv_desc,
            handle
            );
    }
    return;
}

void RenderTargetBackBuffer::StartRender(ID3D12GraphicsCommandList* const in_command_list, const bool in_allow_clear)
{
    if (_current_state != D3D12_RESOURCE_STATE_RENDER_TARGET)
    {
        // Transition the render target into the correct state to allow for drawing into it.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            _render_target_resource.Get(),
            _current_state,
            D3D12_RESOURCE_STATE_RENDER_TARGET
            );
        in_command_list->ResourceBarrier(
            1,
            &barrier
            );
        _current_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    in_command_list->RSSetViewports(
        1,
        &_screen_viewport
        );
    in_command_list->RSSetScissorRects(
        1,
        &_scissor_rect
        );
    auto render_target_descriptor_handle = _render_target_descriptor->GetCPUHandle();
    if (_depth_stencil_view_descriptor)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE dsv_descriptor = _depth_stencil_view_descriptor->GetCPUHandle();
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

    if (true == in_allow_clear)
    {
        PIXBeginEvent(
            in_command_list,
            PIX_COLOR_DEFAULT,
            L"RenderTargetBackBuffer::Clear"
            );
        if ((nullptr != _render_target_descriptor) && _target_format_data._clear_on_set)
        {
            auto descriptor = _render_target_descriptor->GetCPUHandle();
            in_command_list->ClearRenderTargetView(
                descriptor,
                _target_clear_value.Color,
                0,
                nullptr
                );
        }
        if ((nullptr != _depth_stencil_view_descriptor) && (_target_depth_data._clear_depth_on_set || _target_depth_data\
            ._clear_stencil_on_set))
        {
            const D3D12_CLEAR_FLAGS flag = (D3D12_CLEAR_FLAGS)((_target_depth_data._clear_depth_on_set ? \
                D3D12_CLEAR_FLAG_DEPTH : 0) | (_target_depth_data._clear_stencil_on_set ? D3D12_CLEAR_FLAG_STENCIL : 0))\
                ;
            auto descriptor = _depth_stencil_view_descriptor->GetCPUHandle();
            in_command_list->ClearDepthStencilView(
                descriptor,
                flag,
                _depth_clear_value.DepthStencil.Depth,
                _depth_clear_value.DepthStencil.Stencil,
                0,
                nullptr
                );
        }
        PIXEndEvent(in_command_list);
    }
}

void RenderTargetBackBuffer::EndRender(ID3D12GraphicsCommandList* const in_command_list)
{
    if (_current_state != D3D12_RESOURCE_STATE_PRESENT)
    {
        // Transition the render target to the state that allows it to be presented to the display.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            _render_target_resource.Get(),
            _current_state,
            D3D12_RESOURCE_STATE_PRESENT
            );
        in_command_list->ResourceBarrier(
            1,
            &barrier
            );
        _current_state = D3D12_RESOURCE_STATE_PRESENT;
    }
}

void RenderTargetBackBuffer::GetFormatData(
    DXGI_FORMAT& in_depth_format,
    int& in_render_target_view_format_count,
    const DXGI_FORMAT*& in_render_target_view_format
    ) const
{
    in_depth_format = _target_depth_data._format;
    in_render_target_view_format_count = 1;
    in_render_target_view_format = &_target_format_data._format;
}

const VectorInt2 RenderTargetBackBuffer::GetSize() const
{
    return _back_buffer_size;
}
