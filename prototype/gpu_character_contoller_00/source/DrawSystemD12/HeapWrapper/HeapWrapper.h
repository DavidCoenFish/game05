#pragma once

#include "DrawSystemD12/IResource.h"

	//struct HeapWrapperIndex
	//{
	//   std::shared_ptr<HeapWrapper> m_pHeapWrapper;
	//   int m_index;
	//   int m_length;
	//};

	//static

class HeapWrapperPage;
class DrawSystem;

//https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-descriptor-heaps#descriptor-heap-types
class HeapWrapper : public IResource
{
public:
	HeapWrapper(
		DrawSystem* const pDrawSystem,
		const D3D12_DESCRIPTOR_HEAP_TYPE type, 
		const int frameCount = 1,
		const bool bShaderVisible = false,
		const UINT numDescriptors = 256
		);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const int index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleFrame(const int index, const int frameIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const int index);
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(const int index);

	ID3D12DescriptorHeap* const GetHeap(const int index);

	const int GetFreeIndex(
		ID3D12Device2* const pDevice, 
		const int length = 1
		);
	void FreeIndex(const int index, const int length = 1);

private:
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const pCommandList,
		ID3D12Device2* const pDevice
		) override;

private:
	int m_frameCount;
	std::vector< std::shared_ptr< HeapWrapperPage > >m_arrayPage;
	D3D12_DESCRIPTOR_HEAP_DESC m_desc;

};
