#pragma once
class HeapWrapperItem;

/// Shader constant is reflection of a stuct with visiblity in the shader
struct ConstantBuffer
{
public:
    ConstantBuffer(
        const int in_frame_count,
        const std::shared_ptr < HeapWrapperItem >& in_heap_wrapper_item,
        const std::vector<uint8_t>& in_data,
        const D3D12_SHADER_VISIBILITY in_visiblity
        );
    void DeviceLost();
    void DeviceRestored(ID3D12Device* const in_device);
    void Activate(
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_root_param_index,
        const int in_frame_index
        );

    const D3D12_SHADER_VISIBILITY GetVisiblity() const
    {
        return _visiblity;
    }

    const int GetNum32BitValues() const;

    void UpdateData(
        const void* const in_data,
        const size_t in_data_size
        );

    const size_t GetBufferSize() const
    {
        return _data.size();
    }

    const void* const GetBufferData() const
    {
        return _data.data();
    }

private:
    static const size_t MAX_BACK_BUFFER_COUNT = 3;
    const int _frame_count;
    std::shared_ptr < HeapWrapperItem > _heap_wrapper_item;
    Microsoft::WRL::ComPtr < ID3D12Resource > _constant_buffer_upload_heap[MAX_BACK_BUFFER_COUNT];
    UINT8* _gpu_address[MAX_BACK_BUFFER_COUNT];
    std::vector<uint8_t> _data;
    const D3D12_SHADER_VISIBILITY _visiblity;
};
