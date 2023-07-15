#include "common/common_pch.h"

#include "common/draw_system/shader/constant_buffer_info.h"

ConstantBufferInfo::ConstantBufferInfo(
    const std::vector < uint8_t >&in_data,
    const D3D12_SHADER_VISIBILITY in_visiblity
    ) 
    : visiblity(in_visiblity)
    , data(in_data) {} void ConstantBufferInfo::UpdateData(
        const void* const in_data,
        const size_t in_data_size
        )
{
    in_data_size;
    assert(in_data_size == data.in_size());
    memcpy(
        data.in_data(),
        in_data,
        data.in_size()
        );
    return;
}

