#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_page.h"

std::shared_ptr < HeapWrapperPage > HeapWrapperPage::Factory(
	const D3D12_DESCRIPTOR_HEAP_DESC&in_desc,
	ID3D12Device2* const in_device
	)
{
	auto result = std::make_shared < HeapWrapperPage > (
		in_desc.NumDescriptors
		);
	result->OnDeviceRestored(
		in_desc,
		in_device
		);
	return result;
}

HeapWrapperPage::HeapWrapperPage(
	const int in_num_descriptors 
	// Const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pDescriptorHeap,
	// Const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHeapStart,
	// Const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHeapStart

	) 
	: _descriptor_heap()// PDescriptorHeap)

	, _handle_increment_size(0)
	, _cpu_heap_start()// CPUHeapStart)

	, _gpu_heap_start()// GPUHeapStart)

	, _index_high_water(0)
	, _free_index()
	, _num_descriptors(in_num_descriptors)
{
	return;
}

void HeapWrapperPage::OnDeviceLost()
{
	_descriptor_heap.Reset();
}

void HeapWrapperPage::OnDeviceRestored(
	const D3D12_DESCRIPTOR_HEAP_DESC&in_desc,
	ID3D12Device2* const in_device
	)
{
	if (nullptr == in_device)
	{
		return;
	}
	if (nullptr != _descriptor_heap)
	{
		return;
	}
	DX::ThrowIfFailed(in_device->CreateDescriptorHeap(
		&in_desc,
		IID_PPV_ARGS(_descriptor_heap.ReleaseAndGetAddressOf())
		));
	_cpu_heap_start = _descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	_gpu_heap_start = _descriptor_heap->GetGPUDescriptorHandleForHeapStart();

	_handle_increment_size = in_device->GetDescriptorHandleIncrementSize(in_desc.Type);
}

const int HeapWrapperPage::GetFreeIndex(const int in_length)
{
	if ((1 == in_length) && (0 < _free_index.size()))
	{
		int index = _free_index.back();
		_free_index.pop_back();
		return index;
	}
	if (_index_high_water + in_length <= _num_descriptors)
	{
		int result = _index_high_water;
		_index_high_water += in_length;
		return result;
	}
	return - 1;
}

void HeapWrapperPage::FreeIndex(
	const int in_trace,
	const int in_length
	)
{
	for (int index = 0; index < in_length;++ index)
	{
		_free_index.push_back(in_trace + index);
	}
	if (_num_descriptors == _free_index.size())
	{
		_index_high_water = 0;
		_free_index.clear();
	}
	return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetCPUHandle(
	const int in_index
	)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		_cpu_heap_start,
		in_index,
		_handle_increment_size
		);
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetGPUHandle(
	const int in_index
	)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(
		_gpu_heap_start,
		in_index,
		_handle_increment_size
		);
}

ID3D12DescriptorHeap* const HeapWrapperPage::GetHeap()
{
	return _descriptor_heap.Get();
}

