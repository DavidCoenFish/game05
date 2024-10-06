#pragma once
#include "common/draw_system/i_resource.h"

class HeapWrapperItem;

class UnorderedAccess : public IResource
{
public:
	UnorderedAccess(
		DrawSystem* const in_draw_system,
		const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item,
		const std::shared_ptr<HeapWrapperItem>& in_shader_view_heap_wrapper_or_null,
		const D3D12_RESOURCE_DESC& in_desc,
		const D3D12_UNORDERED_ACCESS_VIEW_DESC& in_unordered_access_view_desc,
		const std::vector<uint8_t>& in_data
		);
	std::shared_ptr<HeapWrapperItem> GetHeapWrapperItem() const;
	std::shared_ptr<HeapWrapperItem> GetShaderViewHeapWrapperItem() const;

private:
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const in_command_list,
		ID3D12Device2* const in_device
		) override;
	virtual void OnResourceBarrier(
		ID3D12GraphicsCommandList* const in_command_list,
		D3D12_RESOURCE_STATES in_new_state
		) override;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> _resource;
	std::shared_ptr<HeapWrapperItem> _heap_wrapper_item;
	std::shared_ptr<HeapWrapperItem> _shader_view_heap_wrapper_item;

	D3D12_RESOURCE_DESC _desc;
	D3D12_UNORDERED_ACCESS_VIEW_DESC _unordered_access_view_desc;
	std::vector<uint8_t> _data;

	D3D12_RESOURCE_STATES _current_state;

};
