#include "common/common_pch.h"

#include "common/draw_system/shader/constant_buffer_info.h"
#include "common/json/json_constant_buffer_info.h"
#include "common/json/json_draw_system_enum.h"

const std::vector < std::shared_ptr < ConstantBufferInfo > > TransformConstantBufferInfo(const std::vector < \
    JSONConstantBufferInfo >&in_input)
{
    std::vector < std::shared_ptr < ConstantBufferInfo > > output;
    for (const auto&item : in_input)
    {
        std::vector < uint8_t > data;
        data.resize(sizeof (float) * item._float_count);
        output.push_back(std::make_shared < ConstantBufferInfo > (
            data,
            item._visiblity
            ));
    }
    return output;
}

