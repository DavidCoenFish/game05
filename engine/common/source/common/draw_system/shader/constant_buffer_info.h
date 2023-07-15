#pragma once
struct ConstantBufferInfo
{
public:
    explicit ConstantBufferInfo(
        const std::vector < uint8_t >&in_data = std::vector < uint8_t > (),
        const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
        );
    void UpdateData(
        const void* const in_data,
        const size_t in_data_size
        );
    const D3D12_SHADER_VISIBILITY GetVisiblity() const
    {
        return visiblity;
    }

    const size_t GetBufferSize() const
    {
        return data.size();
    }

    const void* const GetBufferData() const
    {
        return data.data();
    }

    template < class TYPE > TYPE&GetBufferDataAsType()
    {
        assert(sizeof (TYPE) == GetBufferSize());
        TYPE* data =&data[0];
        return* data;
    }
    // Public for json reflection
    private 
        : D3D12_SHADER_VISIBILITY _visiblity;

public:
    std::_vector < uint8_t > _data;
};
