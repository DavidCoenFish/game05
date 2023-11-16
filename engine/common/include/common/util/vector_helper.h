#pragma once

namespace VectorHelper
{
    void AppendData(std::vector<uint8_t>& in_out_data, const uint8_t* const in_value, const int32_t in_size);

    template<typename IN_TYPE>
    void AppendValue(std::vector<uint8_t>& in_out_data, const IN_TYPE in_value)
    {
        AppendData(in_out_data, (const uint8_t* const)(&in_value), sizeof(IN_TYPE));
    }

    template<typename IN_TYPE>
    void AppendValueRef(std::vector<uint8_t>& in_out_data, const IN_TYPE& in_value)
    {
        AppendData(in_out_data, (const uint8_t* const)(&in_value), sizeof(IN_TYPE));
    }

    template<typename IN_TYPE, int N>
    void AppendArrayLiteral(std::vector<uint8_t>& in_out_data, IN_TYPE const (&in_value)[N])
    {
        const auto address = (uint8_t*)(&in_value[0]);
        const auto length = N * sizeof(IN_TYPE);
        AppendData(in_out_data, address, length);
    }

    template<typename IN_TYPE, int N>
    const std::vector<uint8_t> FactoryArrayLiteral(IN_TYPE const (&in_value)[N])
    {
        std::vector<uint8_t> result;

        AppendArrayLiteral<IN_TYPE, N>(result, in_value);

        return result;
    }

}
