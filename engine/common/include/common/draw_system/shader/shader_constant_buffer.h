#pragma once
#include "common/draw_system/i_resource.h"
#include "common/draw_system/shader/constant_buffer.h"

struct ConstantBuffer;

class ShaderConstantBuffer : public IResource
{
public:
    ShaderConstantBuffer(
        DrawSystem* const in_draw_system,
        const std::vector<std::shared_ptr<ConstantBuffer>>& in_array_constant_buffer
        );
    virtual ~ShaderConstantBuffer();

    void SetActive(
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_frame_index,
        int& in_out_root_paramter_index
        );

    void SetConstantBufferData(
        const int in_index,
        const std::vector<float>& in_data
        );
    template<typename CONSTANTS> CONSTANTS& GetConstant(
        const int in_index
        )
    {
        if ((0 <= in_index) && (in_index < _array_constant_buffer.size()))
        {
            auto& constants_buffer = *_array_constant_buffer[in_index];
            assert(sizeof(CONSTANTS) == constants_buffer.GetBufferSize());
            const void* const data = constants_buffer.GetBufferData();
            return (*((CONSTANTS*)data));
        }
        static CONSTANTS result;
        return result;
    }

private:
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const,
        ID3D12Device2* const in_device
        ) override;

private:
    std::vector<std::shared_ptr<ConstantBuffer>> _array_constant_buffer;

};
