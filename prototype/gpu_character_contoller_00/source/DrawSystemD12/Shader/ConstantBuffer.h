#pragma once

class HeapWrapperItem;

/*
shader constant is reflection of a stuct with visiblity in the shader
pData has ownership in ConstantBufferInfo
*/
//either derrive off iResource, or be told by parent that DeviceLost/DeviceRestored
struct ConstantBuffer
{
public:
	ConstantBuffer(
		const int frameCount, 
		const size_t constantBufferSize,
		const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
		const void* const pData,
		const D3D12_SHADER_VISIBILITY visiblity
		);

	void DeviceLost();
	void DeviceRestored(ID3D12Device* const pDevice);

	void Activate(
		ID3D12GraphicsCommandList* const pCommandList,
		//void* const pData, 
		const int rootParamIndex,
		const int frameIndex,
		const bool computeShader
		);

	//void SetRootParamIndex( const int index ) { m_rootParamIndex = index; }
	const D3D12_SHADER_VISIBILITY GetVisiblity() const { return m_visiblity; }
	const int GetNum32BitValues() const;

private:
	static const size_t MAX_BACK_BUFFER_COUNT = 3;
	const int m_frameCount;
	const size_t m_constantBufferSize;
	std::shared_ptr< HeapWrapperItem > m_pHeapWrapperItem;
	//int m_rootParamIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBufferUploadHeap[MAX_BACK_BUFFER_COUNT];
	UINT8* m_pGPUAddress[MAX_BACK_BUFFER_COUNT];
	const void* const m_pData;
	const D3D12_SHADER_VISIBILITY m_visiblity;

};
