#pragma once

class HeapWrapper;
class DrawSystem;

class HeapWrapperItem
{
public:
	static std::shared_ptr<HeapWrapperItem> Factory(
		ID3D12Device2* const pDevice,
		const std::shared_ptr<HeapWrapper>& pHeapWrapper,
		const int length = 1
		);
	HeapWrapperItem(
		const std::shared_ptr<HeapWrapper>& pHeapWrapper,
		const int index,
		const int length
		);
	~HeapWrapperItem();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleFrame(const int frameIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleFrame(const int frameIndex);

	//D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();

	ID3D12DescriptorHeap* const GetHeap();
	const int GetLength() const { return m_length; };

private:
	std::shared_ptr<HeapWrapper> m_pHeapWrapper;
	int m_index;
	int m_length;

};
