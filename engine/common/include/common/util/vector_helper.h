#pragma once

namespace VectorHelper
{
    template<typename IN_TYPE>
    void AppendValue(std::vector<uint8_t>& in_out_data, const IN_TYPE in_value)
    {
        const auto address = (uint8_t*)(&in_value);
        const auto length = sizeof(IN_TYPE);
        for (int index = 0; index < length; ++index)
        {
            in_out_data.push_back(address[index]);
        }
    }

    template<typename IN_TYPE>
    void AppendValueRef(std::vector<uint8_t>& in_out_data, const IN_TYPE& in_value)
    {
        const auto address = (uint8_t*)(&in_value);
        const auto length = sizeof(IN_TYPE);
        for (int index = 0; index < length; ++index)
        {
            in_out_data.push_back(address[index]);
        }
    }

    template<typename IN_TYPE, int N>
    void AppendArrayLiteral(std::vector<uint8_t>& in_out_data, IN_TYPE const (&in_value)[N])
    {
        const auto address = (uint8_t*)(&in_value[0]);
        const auto length = N * sizeof(IN_TYPE);
        for (int index = 0; index < length; ++index)
        {
            in_out_data.push_back(address[index]);
        }
    }

    template<typename IN_TYPE, int N>
    const std::vector<uint8_t> FactoryArrayLiteral(IN_TYPE const (&in_value)[N])
    {
        std::vector<uint8_t> result;

        AppendArrayLiteral<IN_TYPE, N>(result, in_value);

        return result;
    }

}
