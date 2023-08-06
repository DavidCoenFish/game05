#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/render_target/render_target_texture.h"

RenderTargetTexture::Resource::Resource(
    const D3D12_CLEAR_VALUE&in_clear_value,
    const DXGI_FORMAT format,
    const Microsoft::WRL::ComPtr < ID3D12Resource >&in_render_target,
    const std::shared_ptr < HeapWrapperItem >&in_render_target_view_descriptor,
    const std::shared_ptr < HeapWrapperItem >&in_shader_resource_view_descriptor,
    const bool in_clear_color,
    const bool in_clear_depth,
    const bool in_clear_stencil
    ) : _format(format), _render_target(in_render_target), _render_target_view_descriptor(\
    in_render_target_view_descriptor), _shader_resource_view_descriptor(in_shader_resource_view_descriptor), \
    _clear_value{ in_clear_value}
,
_clear_color(in_clear_color)
, _clear_depth(in_clear_depth)
, _clear_stencil(in_clear_stencil)
{
    return;
}

RenderTargetTexture::RenderTargetTexture(
    DrawSystem* const in_draw_system,
    const std::vector < RenderTargetFormatData >&in_target_format_data_array,
    const RenderTargetDepthData&in_target_depth_data,
    const VectorInt2& in_size,
    const bool in_resize_with_screen
    ) 
    : IRenderTarget()
    , IResource(in_draw_system)
    , _size(in_size)
    , _screen_viewport{ 
        0.0f, 
        0.0f,
        static_cast<float>(in_size[0]), 
        static_cast<float>(in_size[1]), 
        D3D12_MIN_DEPTH, 
        D3D12_MAX_DEPTH}
    , _scissor_rect{ 0, 0, in_size[0], in_size[1]}
    , _current_state_render_target(D3D12_RESOURCE_STATE_COMMON)
    , _current_state_depth_resource(D3D12_RESOURCE_STATE_COMMON)
    , _target_format_array()
    , _resize_with_screen(in_resize_with_screen)
{
    for (const auto& iter : in_target_format_data_array)
    {
        D3D12_CLEAR_VALUE clear_value = iter.MakeClearValue();
        _target_resource_array.push_back(std::make_shared < Resource > (
            clear_value,
            iter._format,
            nullptr,
            in_draw_system->MakeHeapWrapperRenderTargetView(),
            in_draw_system->MakeHeapWrapperCbvSrvUav(),
            iter._clear_on_set
            ));
        _target_format_array.push_back(iter._format);
    }
    if (in_target_depth_data._format != DXGI_FORMAT_UNKNOWN)
    {
        D3D12_CLEAR_VALUE clear_value = in_target_depth_data.MakeClearValue();
        _depth_resource = std::make_shared < Resource > (
            clear_value,
            in_target_depth_data._format,
            nullptr,
            in_draw_system->MakeHeapWrapperDepthStencilView(),
            in_target_depth_data._shader_resource ? in_draw_system->MakeHeapWrapperCbvSrvUav() : nullptr,
            false,
            in_target_depth_data._clear_depth,
            in_target_depth_data._clear_stencil
            );
    }
    return;
}

RenderTargetTexture::~RenderTargetTexture()
{
    return;
}

std::shared_ptr < HeapWrapperItem > RenderTargetTexture::GetShaderResourceHeapWrapperItem(const int in_index) const
{
    if ((0 <= in_index) && (in_index < (int) _target_resource_array.size()))
    {
        return _target_resource_array[in_index]->_shader_resource_view_descriptor;
    }
    return nullptr;
}

std::shared_ptr < HeapWrapperItem > RenderTargetTexture::GetDepthResourceHeapWrapperItem() const
{
    if (_depth_resource)
    {
        return _depth_resource->_render_target_view_descriptor;
    }
    return nullptr;
}

std::shared_ptr < HeapWrapperItem > RenderTargetTexture::GetDepthShaderResourceHeapWrapperItem() const
{
    if (_depth_resource)
    {
        return _depth_resource->_shader_resource_view_descriptor;
    }
    return nullptr;
}

void RenderTargetTexture::Resize(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device,
    const VectorInt2& in_size
    )
{
    if (_size == in_size)
    {
        return;
    }
    OnDeviceLost();
    _size = in_size;
    _scissor_rect.right = in_size[0];
    _scissor_rect.bottom = in_size[1];
    _screen_viewport.Width = (float)in_size[0];
    _screen_viewport.Height = (float)in_size[1];
    OnDeviceRestored(
        in_command_list,
        in_device
        );
}

const VectorInt2 RenderTargetTexture::GetSize() const
{
    return _size;
}

void RenderTargetTexture::OnDeviceLost()
{
    for (auto iter : _target_resource_array)
    {
        iter->_render_target.Reset();
    }
    if (nullptr != _depth_resource)
    {
        _depth_resource->_render_target.Reset();
    }
}

void RenderTargetTexture::OnDeviceRestored(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const in_device
    )
{
    // So, is this a good idea, to allow Resize(), call OnDeviceLost
    // OnDeviceLost();
    _array_render_target_descriptors.clear();
    _current_state_render_target = D3D12_RESOURCE_STATE_RENDER_TARGET;
    auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    for (auto iter : _target_resource_array)
    {
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
            iter->_format,
            static_cast<UINT64>(_size[0]),
            static_cast<UINT>(_size[1]),
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
            );
        DX::ThrowIfFailed(in_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
            &desc,
            _current_state_render_target,
            &iter->_clear_value,
            IID_PPV_ARGS(iter->_render_target.ReleaseAndGetAddressOf())
            ));
        iter->_render_target->SetName(L"RenderTargetResource");
        auto render_target_view_descriptor = iter->_render_target_view_descriptor->GetCPUHandle();
        _array_render_target_descriptors.push_back(render_target_view_descriptor);
        in_device->CreateRenderTargetView(
            iter->_render_target.Get(),
            nullptr,
            render_target_view_descriptor
            );
        const int frame_count = _draw_system->GetBackBufferCount();
        for (int index = 0; index < frame_count;++ index)
        {
            in_device->CreateShaderResourceView(
                iter->_render_target.Get(),
                nullptr,
                iter->_shader_resource_view_descriptor->GetCPUHandleFrame(index)
                );
        }
    }
    _current_state_depth_resource = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    if (nullptr != _depth_resource)
    {
        D3D12_RESOURCE_DESC depth_stencil_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            _depth_resource->_format,
            _size[0],
            _size[1],
            1,
            // This depth stencil view has only one texture.
            1,
            // Use a single mipmap level.
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
            );
        DX::ThrowIfFailed(in_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &depth_stencil_desc,
            _current_state_depth_resource,
            &_depth_resource->_clear_value,
            IID_PPV_ARGS(_depth_resource->_render_target.ReleaseAndGetAddressOf())
            ));
        _depth_resource->_render_target->SetName(L"RenderTarget Depth Stencil");

        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
            dsv_desc.Format = _depth_resource->_format;
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            in_device->CreateDepthStencilView(
                _depth_resource->_render_target.Get(),
                &dsv_desc,
                _depth_resource->_render_target_view_descriptor->GetCPUHandle()
                );
        }
        if (nullptr != _depth_resource->_shader_resource_view_descriptor)
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
            srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MipLevels = 1;
            srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            const int frame_count = _draw_system->GetBackBufferCount();
            for (int index = 0; index < frame_count;++ index)
            {
                in_device->CreateShaderResourceView(
                    _depth_resource->_render_target.Get(),
                    &srv_desc,
                    _depth_resource->_shader_resource_view_descriptor->GetCPUHandleFrame(index)
                    );
            }
        }
    }
    return;
}

void RenderTargetTexture::OnResize(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device,
    const VectorInt2& in_screen_size
    )
{
    if (false == _resize_with_screen)
    {
        return;
    }
    Resize(
        in_command_list,
        in_device,
        in_screen_size
        );
    return;
}

void RenderTargetTexture::StartRender(ID3D12GraphicsCommandList* const in_command_list)
{
    TransitionResource(
        in_command_list,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
        );
    in_command_list->RSSetViewports(
        1,
        &_screen_viewport
        );
    in_command_list->RSSetScissorRects(
        1,
        &_scissor_rect
        );
    UINT NumRenderTargetDescriptors = 0;
    const D3D12_CPU_DESCRIPTOR_HANDLE* render_target_descriptors = nullptr;
    const D3D12_CPU_DESCRIPTOR_HANDLE* depth_stencil_descriptor = nullptr;
    if (0 < _array_render_target_descriptors.size())
    {
        NumRenderTargetDescriptors = (UINT) _array_render_target_descriptors.size();
        render_target_descriptors = _array_render_target_descriptors.data();
    }
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_descriptor = _depth_resource ? _depth_resource->_render_target_view_descriptor->\
        GetCPUHandle() : D3D12_CPU_DESCRIPTOR_HANDLE();
    if (_depth_resource)
    {
        depth_stencil_descriptor =&dsv_descriptor;
    }
    in_command_list->OMSetRenderTargets(
        NumRenderTargetDescriptors,
        render_target_descriptors,
        FALSE,
        depth_stencil_descriptor
        );

    {
        PIXBeginEvent(
            in_command_list,
            PIX_COLOR_DEFAULT,
            L"RenderTargetTexture::StartRender::Clear"
            );
        for (const auto&iter : _target_resource_array)
        {
            if (false == iter->_clear_color)
            {
                continue;
            }
            auto descriptor = iter->_render_target_view_descriptor->GetCPUHandle();
            in_command_list->ClearRenderTargetView(
                descriptor,
                iter->_clear_value.Color,
                0,
                nullptr
                );
        }
        if (_depth_resource && (_depth_resource->_clear_depth || _depth_resource->_clear_stencil))
        {
            const D3D12_CLEAR_FLAGS flag = (D3D12_CLEAR_FLAGS)((_depth_resource->_clear_depth ? D3D12_CLEAR_FLAG_DEPTH :\
            0) | (_depth_resource->_clear_stencil ? D3D12_CLEAR_FLAG_STENCIL : 0));
            auto descriptor = _depth_resource->_render_target_view_descriptor->GetCPUHandle();
            in_command_list->ClearDepthStencilView(
                descriptor,
                flag,
                _depth_resource->_clear_value.DepthStencil.Depth,
                _depth_resource->_clear_value.DepthStencil.Stencil,
                0,
                nullptr
                );
        }
        PIXEndEvent(in_command_list);
    }
    return;
}

void RenderTargetTexture::EndRender(ID3D12GraphicsCommandList* const in_command_list)
{
    TransitionResource(
        in_command_list,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_READ
        );
    return;
}

void RenderTargetTexture::TransitionResource(
    ID3D12GraphicsCommandList* const in_command_list,
    const D3D12_RESOURCE_STATES in_new_state_render_target,
    const D3D12_RESOURCE_STATES in_new_state_depth_resource
    )
{
    if (_current_state_render_target != in_new_state_render_target)
    {
        for (const auto&iter : _target_resource_array)
        {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                iter->_render_target.Get(),
                _current_state_render_target,
                in_new_state_render_target
                );
            in_command_list->ResourceBarrier(
                1,
                &barrier
                );
        }
        _current_state_render_target = in_new_state_render_target;
    }
    if (_current_state_depth_resource != in_new_state_depth_resource)
    {
        if (_depth_resource)
        {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _depth_resource->_render_target.Get(),
                _current_state_depth_resource,
                in_new_state_depth_resource
                );
            in_command_list->ResourceBarrier(
                1,
                &barrier
                );
        }
        _current_state_depth_resource = in_new_state_depth_resource;
    }
}

void RenderTargetTexture::GetFormatData(
    DXGI_FORMAT& in_depth_format,
    int& in_render_target_view_format_count,
    const DXGI_FORMAT*& in_render_target_view_format
    ) const
{
    in_depth_format = _depth_resource ? _depth_resource->_format : DXGI_FORMAT_UNKNOWN;
    in_render_target_view_format_count = (int) _target_format_array.size();
    in_render_target_view_format = (DXGI_FORMAT*)(_target_format_array.data());
    return;
}

