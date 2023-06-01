#include "pch.h"

#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapper.h"

std::shared_ptr<HeapWrapperItem> HeapWrapperItem::Factory(
	ID3D12Device2* const pDevice,
	const std::shared_ptr<HeapWrapper>& pHeapWrapper,
	const int length
	)
{
	const int index = pHeapWrapper->GetFreeIndex(pDevice, length );
	return std::make_shared< HeapWrapperItem >(
		pHeapWrapper,
		index,
		length
		);
}

HeapWrapperItem::HeapWrapperItem(
	const std::shared_ptr<HeapWrapper>& pHeapWrapper,
	const int index,
	const int length
	)
	: m_pHeapWrapper( pHeapWrapper )
	, m_index( index )
	, m_length( length )
{
	return;
}

HeapWrapperItem::~HeapWrapperItem()
{
	if (m_pHeapWrapper)
	{
		m_pHeapWrapper->FreeIndex( m_index, m_length );
	}
	return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandle()
{
	if (m_pHeapWrapper)
	{
		return m_pHeapWrapper->GetCPUHandle( m_index );
	}
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}
D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandleFrame(const int frameIndex)
{
	if (m_pHeapWrapper)
	{
		return m_pHeapWrapper->GetCPUHandleFrame( m_index, frameIndex );
	}
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetGPUHandle()
{
	if (m_pHeapWrapper)
	{
		return m_pHeapWrapper->GetGPUHandle( m_index );
	}
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetGPUHandleFrame(const int frameIndex)
{
	if (m_pHeapWrapper)
	{
		return m_pHeapWrapper->GetGPUHandleFrame( m_index, frameIndex );
	}
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

//D3D12_GPU_VIRTUAL_ADDRESS HeapWrapperItem::GetGPUAddress()
//{
//	if (m_pHeapWrapper)
//	{
//		return m_pHeapWrapper->GetGPUAddress( m_index );
//	}
//	return D3D12_GPU_VIRTUAL_ADDRESS();
//}

ID3D12DescriptorHeap* const HeapWrapperItem::GetHeap()
{
	if (m_pHeapWrapper)
	{
		return m_pHeapWrapper->GetHeap( m_index );
	}
	return nullptr;
}
