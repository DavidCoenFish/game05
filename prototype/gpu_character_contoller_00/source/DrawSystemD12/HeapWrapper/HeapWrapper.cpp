#include "pch.h"

#include "DrawSystemD12/HeapWrapper/HeapWrapper.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperPage.h"
#include "DrawSystemD12/DrawSystem.h"

HeapWrapper::HeapWrapper(
	DrawSystem* const pDrawSystem,
	const D3D12_DESCRIPTOR_HEAP_TYPE type, 
	const int frameCount,
	const bool bShaderVisible,
	const UINT numDescriptors
	)
	: IResource(pDrawSystem)
	, m_frameCount(frameCount)
	, m_desc()
{
	m_desc.Type = type;
	m_desc.NumDescriptors = numDescriptors;
	m_desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapper::GetCPUHandle(const int index)
{
	const int frameIndex = m_pDrawSystem ? m_pDrawSystem->GetBackBufferIndex() : 0;
	return GetCPUHandleFrame(index, frameIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapper::GetCPUHandleFrame(const int index, const int frameIndex)
{
	const int pageIndex = index / m_desc.NumDescriptors;
	if ((0 <= pageIndex) && (pageIndex < (int)m_arrayPage.size()))
	{
		const int localIndex = index - (m_desc.NumDescriptors * pageIndex);
		return m_arrayPage[ pageIndex ]->GetCPUHandle(localIndex, frameIndex);
	}
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapper::GetGPUHandle(const int index)
{
	assert(m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE); 
	const int frameIndex = m_pDrawSystem ? m_pDrawSystem->GetBackBufferIndex() : 0;
	const int pageIndex = index / m_desc.NumDescriptors;
	if ((0 <= pageIndex) && (pageIndex < (int)m_arrayPage.size()))
	{
		const int localIndex = index - (m_desc.NumDescriptors * pageIndex);
		return m_arrayPage[ pageIndex ]->GetGPUHandle(localIndex, frameIndex);
	}
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

ID3D12DescriptorHeap* const HeapWrapper::GetHeap(const int index)
{
	const int frameIndex = m_pDrawSystem ? m_pDrawSystem->GetBackBufferIndex() : 0;
	const int pageIndex = index / m_desc.NumDescriptors;
	if ((0 <= pageIndex) && (pageIndex < (int)m_arrayPage.size()))
	{
		return m_arrayPage[ pageIndex ]->GetHeap(frameIndex);
	}
	return nullptr;
}

const int HeapWrapper::GetFreeIndex(
	ID3D12Device2* const pDevice, 
	const int length)
{
	int trace = 0;
	for (auto iter : m_arrayPage)
	{
		const int result = iter->GetFreeIndex(length);
		if (-1 != result)
		{
			return result + trace;
		}
		trace += m_desc.NumDescriptors;
	}

	{
		auto pPage = HeapWrapperPage::Factory( m_frameCount, m_desc, pDevice );
		const int result = pPage->GetFreeIndex(length);
		m_arrayPage.push_back(pPage);
		return result + trace;
	}
}

void HeapWrapper::FreeIndex(const int index, const int length)
{
	const int pageIndex = index / m_desc.NumDescriptors;
	if ((0 <= pageIndex) && (pageIndex < (int)m_arrayPage.size()))
	{
		const int localIndex = index - (m_desc.NumDescriptors * pageIndex);
		m_arrayPage[ pageIndex ]->FreeIndex(localIndex, length);
	}
	return;
}

void HeapWrapper::OnDeviceLost()
{
	for (auto iter : m_arrayPage)
	{
		iter->OnDeviceLost();
	}
	return;
}

void HeapWrapper::OnDeviceRestored(
	ID3D12GraphicsCommandList* const,
	ID3D12Device2* const pDevice
	)
{
	for (auto iter : m_arrayPage)
	{
		iter->OnDeviceRestored(m_desc, pDevice);
	}
	return;
}
