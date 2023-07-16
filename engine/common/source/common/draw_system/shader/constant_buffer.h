#pragma once
class HeapWrapperItem;

/*
    shader constant is reflection of a stuct with visiblity in the shader
    pData has ownership in ConstantBufferInfo
*/
// Either derrive off iResource, or be told by parent that DeviceLost/DeviceRestored
struct ConstantBuffer
{
public:
    ConstantBuffer(
        const int in_frame_count,
        const size_t in_constant_buffer_size,
        const std::shared_ptr < HeapWrapperItem >&in_heap_wrapper_item,
        const void* const in_data,
        const D3D12_SHADER_VISIBILITY in_visiblity
        );
    void DeviceLost();
    void DeviceRestored(ID3D12Device* const in_device);
    void Activate(
        ID3D12GraphicsCommandList* const in_command_list,
        // Void* const pData,
        const int in_root_param_index,
        const int in_frame_index
        );
    // Void SetRootParamIndex( const int index ) { m_rootParamIndex = index; }
    const D3D12_SHADER_VISIBILITY GetVisiblity() const
    {
        return _visiblity;
    }

    const int GetNum32BitValues() const;

private:
    static const size_t MAX_BACK_BUFFER_COUNT = 3;
    const int _frame_count;
    const size_t _constant_buffer_size;
    std::shared_ptr < HeapWrapperItem > _heap_wrapper_item;
    // Int m_rootParamIndex;
    Microsoft::WRL::ComPtr < ID3D12Resource > _constant_buffer_upload_heap[MAX_BACK_BUFFER_COUNT];
    UINT8* _gpu_address[MAX_BACK_BUFFER_COUNT];
    const void* const _data;
    const D3D12_SHADER_VISIBILITY _visiblity;
};
