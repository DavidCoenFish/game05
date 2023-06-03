#include "pch.h"

#include "DrawSystemD12/Shader/UnorderedAccess.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/d3dx12.h"
//#include "DirectXTK12/DirectXHelpers.h"
//#include "DirectXTK12/GraphicsMemory.h"

UnorderedAccess::UnorderedAccess(
	DrawSystem* const pDrawSystem,
	const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
	const D3D12_RESOURCE_DESC& desc, 
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc,
	const std::shared_ptr< HeapWrapperItem >& pShaderViewHeapWrapperOrNull
	)
	: IResource( pDrawSystem )
	, m_pHeapWrapperItem(pHeapWrapperItem)
	, m_desc(desc)
	, m_unorderedAccessViewDesc(unorderedAccessViewDesc)
	, m_currentState(D3D12_RESOURCE_STATE_COMMON)
	, m_pShaderViewHeapWrapperItem(pShaderViewHeapWrapperOrNull)
{
	return;
}

void UnorderedAccess::OnDeviceLost()
{
	m_pResource.Reset();
}

std::shared_ptr< HeapWrapperItem > UnorderedAccess::GetHeapWrapperItem() const
{
	return m_pHeapWrapperItem;
}

std::shared_ptr< HeapWrapperItem > UnorderedAccess::GetShaderViewHeapWrapperItem() const
{
	return m_pShaderViewHeapWrapperItem;
}

void UnorderedAccess::OnResourceBarrier(
	ID3D12GraphicsCommandList* const pCommandList,
	D3D12_RESOURCE_STATES newState
	)
{
	if (newState == m_currentState)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource = m_pResource.Get();
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = m_currentState;
	barrierDesc.Transition.StateAfter = newState;

	pCommandList->ResourceBarrier(1, &barrierDesc);
	m_currentState = newState;

}

void UnorderedAccess::OnDeviceRestored(
	ID3D12GraphicsCommandList* const,
	ID3D12Device2* const pDevice
	)
{
	m_currentState = D3D12_RESOURCE_STATE_COMMON;

	CD3DX12_HEAP_PROPERTIES heapDefault(D3D12_HEAP_TYPE_DEFAULT);
	pDevice->CreateCommittedResource(
		&heapDefault,
		D3D12_HEAP_FLAG_NONE,
		&m_desc,
		m_currentState,
		nullptr,
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
		);

	pDevice->CreateUnorderedAccessView( 
		m_pResource.Get(), 
		nullptr, 
		&m_unorderedAccessViewDesc, 
		m_pHeapWrapperItem->GetCPUHandleFrame(0)
		);

	pDevice->CreateShaderResourceView(
		m_pResource.Get(), 
		nullptr,
		m_pShaderViewHeapWrapperItem->GetCPUHandleFrame(0)
		);

	return;
}

