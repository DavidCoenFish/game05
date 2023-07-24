#include "common/common_pch.h"

#include "common/direct_xtk12/graphics_memory.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/device_resources.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/unordered_access.h"
#include "common/draw_system/shader/unordered_access_info.h"
#include "common/json/json_draw_system.h"

std::unique_ptr < DrawSystem > DrawSystem::Factory(
    const HWND in_hwnd,
    const JSONDrawSystem& in_json
    )
{
    return std::make_unique < DrawSystem > (
        in_hwnd,
        in_json._back_buffer_count,
        in_json._d3d_feature_level,
        in_json._options,
        in_json._target_format_data,
        in_json._target_depth_data
        );
}

DrawSystem::DrawSystem(
    const HWND in_hwnd,
    const unsigned int in_back_buffer_count,
    const D3D_FEATURE_LEVEL in_d3d_feature_level,
    const unsigned int in_options,
    const RenderTargetFormatData& in_target_format_data,
    const RenderTargetDepthData& in_target_depth_data
    ) 
    : _hwnd(in_hwnd)
    , _back_buffer_count(in_back_buffer_count)
    , _d3d_feature_level(in_d3d_feature_level)
    , _options(in_options)
    , _target_format_data(in_target_format_data)
    , _target_depth_data(in_target_depth_data)
{
    _heap_wrapper_cbv_srv_uav = std::make_shared < HeapWrapper > (
        this,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        in_back_buffer_count,
        true
        );
    _heap_wrapper_sampler = std::make_shared < HeapWrapper > (
        this,
        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
        );
    _heap_wrapper_render_target_view = std::make_shared < HeapWrapper > (
        this,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV
        );
    _heap_wrapper_depth_stencil_view = std::make_shared < HeapWrapper > (
        this,
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV
        );
    CreateDeviceResources();
}

DrawSystem::~DrawSystem()
{
    WaitForGpu();
    for (auto iter : _list_resource)
    {
        iter->OnDeviceLost();
    }
    _list_resource.clear();
    _device_resources.reset();
}

DirectX::GraphicsResource DrawSystem::AllocateConstant(
    const std::size_t in_size,
    void* const in_constants
    )
{
    return AllocateUpload(
        in_size,
        in_constants,
        D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT
        );
}

DirectX::GraphicsResource DrawSystem::AllocateUpload(
    const std::size_t in_size,
    void* const in_data_or_nullptr,
    size_t in_alignment
    )
{
    if (_device_resources)
    {
        return _device_resources->AllocateUpload(
            in_size,
            in_data_or_nullptr,
            in_alignment
            );
    }
    return DirectX::GraphicsResource();
}

std::shared_ptr < Shader > DrawSystem::MakeShader(
    ID3D12GraphicsCommandList* const in_command_list,
    const ShaderPipelineStateData& in_pipeline_state_data,
    const std::shared_ptr<std::vector<uint8_t>>& in_vertex_shader_data,
    const std::shared_ptr<std::vector<uint8_t>>& in_geometry_shader_data,
    const std::shared_ptr<std::vector<uint8_t>>& in_pixel_shader_data,
    const std::vector<std::shared_ptr<ShaderResourceInfo>>& in_array_shader_resource_info,
    const std::vector<std::shared_ptr<ConstantBufferInfo>>& in_array_shader_constants_info,
    const std::shared_ptr<std::vector<uint8_t>>& in_compute_shader_data,
    const std::vector<std::shared_ptr<UnorderedAccessInfo>>& in_array_unordered_access_info
    )
{
    auto result = std::make_shared<Shader>(
        this,
        in_pipeline_state_data,
        in_vertex_shader_data,
        in_geometry_shader_data,
        in_pixel_shader_data,
        in_array_shader_resource_info,
        in_array_shader_constants_info,
        in_compute_shader_data,
        in_array_unordered_access_info
        );
    if (result && _device_resources)
    {
        ((IResource*)(result.get()))->OnDeviceRestored(
            in_command_list,
            _device_resources->GetD3dDevice()
            );
    }
    return result;
}

std::shared_ptr < GeometryGeneric > DrawSystem::MakeGeometryGeneric(
    ID3D12GraphicsCommandList* const in_command_list,
    const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::vector<uint8_t>& in_vertex_data_raw,
    const int in_float_per_vertex
    )
{
    auto result = std::make_shared < GeometryGeneric > (
        this,
        in_primitive_topology,
        in_input_element_desc_array,
        in_vertex_data_raw,
        in_float_per_vertex
        );
    if (result && _device_resources)
    {
        ((IResource*)(result.get()))->OnDeviceRestored(
            in_command_list,
            _device_resources->GetD3dDevice()
            );
    }
    return result;
}

std::shared_ptr < ShaderResource > DrawSystem::MakeShaderResource(
    ID3D12GraphicsCommandList* const in_command_list,
    const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
    const D3D12_RESOURCE_DESC&in_desc,
    const D3D12_SHADER_RESOURCE_VIEW_DESC&in_shader_resource_view_desc,
    const std::vector < uint8_t >&in_data
    )
{
    auto result = std::make_shared < ShaderResource > (
        this,
        in_shader_resource,
        in_desc,
        in_shader_resource_view_desc,
        in_data
        );
    if (result && _device_resources)
    {
        ((IResource*)(result.get()))->OnDeviceRestored(
            in_command_list,
            _device_resources->GetD3dDevice()
            );
    }
    return result;
}

std::shared_ptr < UnorderedAccess > DrawSystem::MakeUnorderedAccess(
    ID3D12GraphicsCommandList* const in_command_list,
    const D3D12_RESOURCE_DESC& in_desc,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC& in_unordered_access_view_desc,
    const bool in_make_shader_view_heap_wrapper_item,
    const std::vector<uint8_t>& in_data
)
{
    auto heap_wrapper_item = MakeHeapWrapperCbvSrvUav();
    auto shader_view_heap_wrapper_or_null = in_make_shader_view_heap_wrapper_item ? MakeHeapWrapperCbvSrvUav() : nullptr;

    auto result = std::make_shared<UnorderedAccess>(
        this,
        heap_wrapper_item,
        shader_view_heap_wrapper_or_null,
        in_desc,
        in_unordered_access_view_desc,
        in_data
        );
    if (result && _device_resources)
    {
        ((IResource*)(result.get()))->OnDeviceRestored(
            in_command_list,
            _device_resources->GetD3dDevice()
            );
    }
    return result;
}

std::shared_ptr < RenderTargetTexture > DrawSystem::MakeRenderTargetTexture(
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector < RenderTargetFormatData >&in_target_format_data_array,
    const RenderTargetDepthData&in_target_depth_data,
    const int in_width,
    const int in_height,
    const bool in_resize_with_screen
    )
{
    auto result = std::make_shared<RenderTargetTexture>(
        this,
        in_target_format_data_array,
        in_target_depth_data,
        in_width,
        in_height,
        in_resize_with_screen
        );
    if (result && _device_resources)
    {
        ((IResource*)(result.get()))->OnDeviceRestored(
            in_command_list,
            _device_resources->GetD3dDevice()
            );
    }
    return result;
}

std::shared_ptr < CustomCommandList > DrawSystem::CreateCustomCommandList(
    ID3D12PipelineState* const in_pipeline_state_object_or_null
    )
{
    if (_device_resources)
    {
        auto command_list = _device_resources->GetCustomCommandList(in_pipeline_state_object_or_null);
        return std::make_shared < CustomCommandList > (
            * this,
            command_list
            );
    }
    return nullptr;
}

void DrawSystem::CustomCommandListFinish(ID3D12GraphicsCommandList* in_command_list)
{
    if (_device_resources)
    {
        _device_resources->CustomCommandListFinish(in_command_list);
    }
    return;
}

std::unique_ptr < DrawSystemFrame > DrawSystem::CreateNewFrame()
{
    return std::make_unique < DrawSystemFrame > (* this);
}

void DrawSystem::Prepare(ID3D12GraphicsCommandList*&in_command_list)
{
    if (nullptr != _device_resources)
    {
        _device_resources->Prepare(in_command_list);
    }
    return;
}

// Void DrawSystem::Clear()
// {
// If (nullptr == m_pDeviceResources)
// {
// Return;
// }
// M_pDeviceResources->Clear();
// }
void DrawSystem::Present()
{
    if (nullptr == _device_resources)
    {
        return;
    }
    if (false == _device_resources->Present())
    {
        CreateDeviceResources();
    }
}

IRenderTarget* DrawSystem::GetRenderTargetBackBuffer()
{
    if (_device_resources)
    {
        return _device_resources->GetRenderTargetBackBuffer();
    }
    return nullptr;
}

std::shared_ptr < HeapWrapperItem > DrawSystem::MakeHeapWrapperCbvSrvUav(const int in_length)
{
    return HeapWrapperItem::Factory(
        _device_resources ? _device_resources->GetD3dDevice() : nullptr,
        _heap_wrapper_cbv_srv_uav,
        in_length
        );
}

std::shared_ptr < HeapWrapperItem > DrawSystem::MakeHeapWrapperSampler(const int in_length)
{
    return HeapWrapperItem::Factory(
        _device_resources ? _device_resources->GetD3dDevice() : nullptr,
        _heap_wrapper_sampler,
        in_length
        );
}

std::shared_ptr < HeapWrapperItem > DrawSystem::MakeHeapWrapperRenderTargetView(const int in_length)
{
    return HeapWrapperItem::Factory(
        _device_resources ? _device_resources->GetD3dDevice() : nullptr,
        _heap_wrapper_render_target_view,
        in_length
        );
}

std::shared_ptr < HeapWrapperItem > DrawSystem::MakeHeapWrapperDepthStencilView(const int in_length)
{
    return HeapWrapperItem::Factory(
        _device_resources ? _device_resources->GetD3dDevice() : nullptr,
        _heap_wrapper_depth_stencil_view,
        in_length
        );
}

void DrawSystem::WaitForGpu() noexcept
{
    if (_device_resources)
    {
        _device_resources->WaitForGpu();
    }
}

void DrawSystem::OnResize()
{
    bool resized = false;
    int width = 0;
    int height = 0;
    if (_device_resources)
    {
        resized = _device_resources->OnResize(
            this,
            _hwnd,
            width,
            height
            );
    }
    if ((true == resized) && (0 < _list_resource.size()))
    {
        auto command_list = CreateCustomCommandList();
        for (auto iter : _list_resource)
        {
            iter->OnResize(
                command_list->GetCommandList(),
                _device_resources->GetD3dDevice(),
                width,
                height
                );
        }
    }
}

void DrawSystem::AddResource(IResource* const in_resource)
{
    if (in_resource)
    {
        _list_resource.push_back(in_resource);
    }
}

void DrawSystem::RemoveResource(IResource* const in_resource)
{
    // If RemoveResource is called by the dtor, then not a good idea to call a virutal func on the pResource?
    // If (pResource)
    // {
    // PResource->OnDeviceLost();
    // }
    _list_resource.remove(in_resource);
}

const int DrawSystem::GetBackBufferIndex() const
{
    if (nullptr != _device_resources)
    {
        _device_resources->GetBackBufferIndex();
    }
    return 0;
}

void DrawSystem::CreateDeviceResources()
{
    for (auto iter : _list_resource)
    {
        iter->OnDeviceLost();
    }
    _device_resources.reset();
    _device_resources = std::make_unique < DeviceResources > (
        2,
        _d3d_feature_level,
        _options,
        _target_format_data,
        _target_depth_data
        );
    // Two pass construction as rendertargetbackbuffer calls MakeHeapWrapperRenderTargetView, MakeHeapWrapperDepthStencilView which need m_pDeviceResources assigned
    _device_resources->CreateWindowSizeDependentResources(
        this,
        _hwnd
        );
    if (0 < _list_resource.size())
    {
        auto command_list = CreateCustomCommandList();
        for (auto iter : _list_resource)
        {
            iter->OnDeviceRestored(
                command_list->GetCommandList(),
                _device_resources->GetD3dDevice()
                );
        }
    }
}

