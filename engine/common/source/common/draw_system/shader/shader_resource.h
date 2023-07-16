#pragma once
#include "common/draw_system/i_resource.h"

class HeapWrapperItem;

/* was ShaderTexture */
class ShaderResource : public IResource
{
public:
    ShaderResource(
        DrawSystem* const in_draw_system,
        const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
        const D3D12_RESOURCE_DESC&in_desc,
        const D3D12_SHADER_RESOURCE_VIEW_DESC&in_shader_resource_view_desc,
        const std::vector < uint8_t >&in_data
        );
    std::shared_ptr < HeapWrapperItem > GetHeapWrapperItem() const;
    static void UploadCreateResource(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device* const in_device,
        Microsoft::WRL::ComPtr < ID3D12Resource >&in_resource,
        std::shared_ptr < HeapWrapperItem >&in_shader_resource,
        const D3D12_RESOURCE_DESC&in_desc,
        const D3D12_SHADER_RESOURCE_VIEW_DESC&in_shader_resource_view_desc,
        const size_t in_data_size,
        void* in_data,
        LPCWSTR in_name
        );

private:
    // Void WriteData(
    // Const int x,
    // Const int y,
    // Const size_t dataSizeBytes,
    // Void* pData
    // );
    // Std::vector<uint8_t>& GetData();
    // Void UploadChangesIfNeeded(
    // ID3D12GraphicsCommandList* const pCommandList
    // );
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) override;

private:
    Microsoft::WRL::ComPtr < ID3D12Resource > _resource;
    std::shared_ptr < HeapWrapperItem > _shader_resource;
    D3D12_RESOURCE_DESC _desc;
    D3D12_SHADER_RESOURCE_VIEW_DESC _shader_resource_view_desc;
    std::vector < uint8_t > _data;
};
