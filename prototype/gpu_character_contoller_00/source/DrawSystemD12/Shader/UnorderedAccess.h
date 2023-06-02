#pragma once

#include "DrawSystemD12/IResource.h"

class HeapWrapperItem;

class UnorderedAccess : public IResource
{
public:
	UnorderedAccess(
		DrawSystem* const pDrawSystem,
		const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
		const D3D12_RESOURCE_DESC& desc, 
		const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc,

		const std::shared_ptr< HeapWrapperItem >& pShaderViewHeapWrapperOrNull = nullptr,
		const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc = D3D12_SHADER_RESOURCE_VIEW_DESC()
		);

	std::shared_ptr< HeapWrapperItem > GetHeapWrapperItem() const;
	std::shared_ptr< HeapWrapperItem > GetShaderViewHeapWrapperItem() const;

private:
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const pCommandList,
		ID3D12Device2* const pDevice
		) override;
	virtual void OnResourceBarrier(
		ID3D12GraphicsCommandList* const pCommandList, 
		D3D12_RESOURCE_STATES afterState
		) override;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
	std::shared_ptr< HeapWrapperItem > m_pHeapWrapperItem;
	D3D12_RESOURCE_DESC m_desc; 
	D3D12_UNORDERED_ACCESS_VIEW_DESC m_unorderedAccessViewDesc;
	D3D12_RESOURCE_STATES m_currentState;

	std::shared_ptr< HeapWrapperItem > m_pShaderViewHeapWrapperItem;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc;

};
