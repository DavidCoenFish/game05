#include "common/common_pch.h"
#include "common/draw_system/shader/shader_constant_buffer.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/i_resource.h"
#include "common/draw_system/shader/constant_buffer.h"
#include "common/draw_system/shader/constant_buffer_info.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/unordered_access_info.h"

ShaderConstantBuffer::ShaderConstantBuffer(
    DrawSystem* const in_draw_system,
    const std::vector<std::shared_ptr<ConstantBuffer>>& _array_constant_buffer
    ) 
    : IResource(in_draw_system)
    , _array_constant_buffer(_array_constant_buffer)
{
    return;
}

ShaderConstantBuffer::~ShaderConstantBuffer()
{
    // Nop
}

void ShaderConstantBuffer::SetActive(
    ID3D12GraphicsCommandList* const in_command_list,
    int& in_out_root_paramter_index
    )
{
    // B0,b1,b2,...
    for (const auto& iter : _array_constant_buffer)
    {
        iter->Activate(
            in_command_list,
            in_out_root_paramter_index
            );
        in_out_root_paramter_index += 1;
    }

    return;
}

void ShaderConstantBuffer::SetConstantBufferData(
    const int in_index,
    const std::vector<float>& in_data
    )
{
    if ((0 <= in_index) && (in_index < (int) _array_constant_buffer.size()))
    {
        auto& constant_buffer =* _array_constant_buffer[in_index];
        const void* const data = &in_data[0];
        constant_buffer.UpdateData(
            data,
            sizeof(float) * in_data.size()
            );
    }
    return;
}

void ShaderConstantBuffer::OnDeviceLost()
{
    for (auto&constant_buffer : _array_constant_buffer)
    {
        constant_buffer->DeviceLost();
    }
}

void ShaderConstantBuffer::OnDeviceRestored(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const in_device
    )
{
    for (auto&constant_buffer : _array_constant_buffer)
    {
        constant_buffer->DeviceRestored(in_device);
    }
    return;
}

