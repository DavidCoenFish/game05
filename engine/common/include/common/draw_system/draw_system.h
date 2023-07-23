#pragma once
#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"

class DeviceResources;
class DrawSystemFrame;
class IResource;
class IRenderTarget;
class HeapWrapper;
class HeapWrapperItem;
class CustomCommandList;
class Shader;
struct ShaderPipelineStateData;
struct ConstantBufferInfo;
struct ShaderResourceInfo;
struct ConstantBuffer;
struct UnorderedAccessInfo;
class GeometryGeneric;
class ShaderResource;
class RenderTargetTexture;
class JSONDrawSystem;
class UnorderedAccess;

namespace DirectX
{
    class GraphicsMemory;
    class GraphicsResource;

};
class DrawSystem
{
public:
    static std::unique_ptr<DrawSystem> Factory(
        const HWND in_hwnd,
        const JSONDrawSystem& in_json
        );
    DrawSystem(
        const HWND in_hwnd,
        const unsigned int in_back_buffer_count = 2,
        const D3D_FEATURE_LEVEL in_d3d_feature_level = D3D_FEATURE_LEVEL_11_0,
        const unsigned int in_options = 0,
        const RenderTargetFormatData& in_target_format_data = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
        const RenderTargetDepthData& in_target_depth_data = RenderTargetDepthData()
        );
    ~DrawSystem();

    void WaitForGpu() noexcept;
    void OnResize();
    void AddResource(IResource* const in_resource);
    void RemoveResource(IResource* const in_resource);
    const int GetBackBufferIndex() const;
    const int GetBackBufferCount() const
    {
        return _back_buffer_count;
    }

    DirectX::GraphicsResource AllocateConstant(
        const std::size_t in_size,
        void* const in_constants
        );
    DirectX::GraphicsResource AllocateUpload(
        const std::size_t in_size,
        void* const in_data_or_nullptr = nullptr,
        size_t in_alignment = 16
        );

    std::shared_ptr<Shader> MakeShader(
        ID3D12GraphicsCommandList* const in_command_list,
        const ShaderPipelineStateData& in_pipeline_state_data,
        const std::shared_ptr<std::vector<uint8_t>>& in_vertex_shader_data,
        const std::shared_ptr<std::vector<uint8_t>>& in_geometry_shader_data,
        const std::shared_ptr<std::vector<uint8_t>>& in_pixel_shader_data,
        const std::vector<std::shared_ptr<ShaderResourceInfo>>& in_array_shader_resource_info = std::vector<std::\
            shared_ptr<ShaderResourceInfo>>(),
        const std::vector<std::shared_ptr<ConstantBufferInfo>>& in_array_shader_constants_info = std::vector<std\
            ::shared_ptr<ConstantBufferInfo>>(),
        const std::shared_ptr<std::vector<uint8_t>>& in_compute_shader_data = std::shared_ptr<std::vector<uint8_t>>(),
        const std::vector<std::shared_ptr<UnorderedAccessInfo>>& in_array_unordered_access_info = std::vector<std\
            ::shared_ptr<UnorderedAccessInfo>>()
        );

    std::shared_ptr<GeometryGeneric> MakeGeometryGeneric(
        ID3D12GraphicsCommandList* const in_command_list,
        const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::vector<float>& in_vertex_data_raw,
        const int in_float_per_vertex
        );

    std::shared_ptr<ShaderResource> MakeShaderResource(
        ID3D12GraphicsCommandList* const in_command_list,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item,
        const D3D12_RESOURCE_DESC& in_desc,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& in_shader_resource_view_desc,
        const std::vector<uint8_t>& in_data = std::vector<uint8_t>()
        );

    std::shared_ptr<UnorderedAccess> MakeUnorderedAccess(
        ID3D12GraphicsCommandList* const in_command_list,
        const D3D12_RESOURCE_DESC& in_desc,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& in_unordered_access_view_desc,
        const bool in_make_shader_view_heap_wrapper_item,
        const std::vector<uint8_t>& in_data = std::vector<uint8_t>()
        );

    std::shared_ptr<RenderTargetTexture> MakeRenderTargetTexture(
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<RenderTargetFormatData>& in_target_format_data_array,
        const RenderTargetDepthData& in_target_depth_data,
        const int in_width,
        const int in_height,
        const bool in_resize_with_screen = false
        );

    std::shared_ptr<CustomCommandList> CreateCustomCommandList(
        ID3D12PipelineState* const in_pipeline_state_object_or_null = nullptr
        );
    void CustomCommandListFinish(ID3D12GraphicsCommandList* in_command_list);

    // Ctor == Prepare, dtor == Present
    std::unique_ptr<DrawSystemFrame> CreateNewFrame();

    void Prepare(ID3D12GraphicsCommandList*& in_command_list);
    void Present();

    IRenderTarget* GetRenderTargetBackBuffer();

    std::shared_ptr<HeapWrapperItem> MakeHeapWrapperCbvSrvUav(const int in_length = 1);
    std::shared_ptr<HeapWrapperItem> MakeHeapWrapperSampler(const int in_length = 1);
    std::shared_ptr<HeapWrapperItem> MakeHeapWrapperRenderTargetView(const int in_length = 1);
    std::shared_ptr<HeapWrapperItem> MakeHeapWrapperDepthStencilView(const int in_length = 1);

private:
    void CreateDeviceResources();

private:
    HWND _hwnd;
    unsigned int _back_buffer_count;
    D3D_FEATURE_LEVEL _d3d_feature_level;
    unsigned int _options;
    RenderTargetFormatData _target_format_data;
    RenderTargetDepthData _target_depth_data;

    std::unique_ptr<DeviceResources> _device_resources;
    std::list<IResource*> _list_resource;

    std::shared_ptr<HeapWrapper> _heap_wrapper_cbv_srv_uav;
    std::shared_ptr<HeapWrapper> _heap_wrapper_sampler;
    std::shared_ptr<HeapWrapper> _heap_wrapper_render_target_view;
    std::shared_ptr<HeapWrapper> _heap_wrapper_depth_stencil_view;

};
