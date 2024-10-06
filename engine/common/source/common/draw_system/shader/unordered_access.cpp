#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/unordered_access.h"
#include "common/draw_system/shader/shader_resource.h"

UnorderedAccess::UnorderedAccess(
	DrawSystem* const in_draw_system,
	const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item,
	const std::shared_ptr<HeapWrapperItem>& in_shader_view_heap_wrapper_or_null,
	const D3D12_RESOURCE_DESC& in_desc,
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& in_unordered_access_view_desc,
	const std::vector<uint8_t>& in_data
)
	: IResource(in_draw_system)
	, _heap_wrapper_item(in_heap_wrapper_item)
	, _shader_view_heap_wrapper_item(in_shader_view_heap_wrapper_or_null)
	, _desc(in_desc)
	, _unordered_access_view_desc(in_unordered_access_view_desc)
	, _data(in_data)
	, _current_state(D3D12_RESOURCE_STATE_COMMON)
{
	return;
}

void UnorderedAccess::OnDeviceLost()
{
	_resource.Reset();
}

std::shared_ptr<HeapWrapperItem> UnorderedAccess::GetHeapWrapperItem() const
{
	return _heap_wrapper_item;
}

std::shared_ptr<HeapWrapperItem> UnorderedAccess::GetShaderViewHeapWrapperItem() const
{
	return _shader_view_heap_wrapper_item;
}

void UnorderedAccess::OnDeviceRestored(
	ID3D12GraphicsCommandList* const in_command_list,
	ID3D12Device2* const in_device
	)
{
	_current_state = D3D12_RESOURCE_STATE_COPY_DEST;

	CD3DX12_HEAP_PROPERTIES heap_default(D3D12_HEAP_TYPE_DEFAULT);
	DX::ThrowIfFailed(in_device->CreateCommittedResource(
		&heap_default,
		D3D12_HEAP_FLAG_NONE,
		&_desc,
		_current_state,
		nullptr,
		IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf())
		));
	_resource->SetName(L"UnorderedAccess resource");

	ShaderResource::UploadResource(
		_draw_system,
		in_command_list,
		_resource,
		_desc,
		_data.size(),
		_data.size() ? _data.data() : nullptr
		);
	OnResourceBarrier(
		in_command_list,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

	in_device->CreateUnorderedAccessView(
		_resource.Get(),
		nullptr,
		&_unordered_access_view_desc,
		_heap_wrapper_item->GetCPUHandleFrame()
		);

	in_device->CreateShaderResourceView(
		_resource.Get(),
		nullptr,
		_shader_view_heap_wrapper_item->GetCPUHandleFrame()
		);

	return;
}

void UnorderedAccess::OnResourceBarrier(
	ID3D12GraphicsCommandList* const in_command_list,
	D3D12_RESOURCE_STATES in_new_state
	)
{
	if (in_new_state == _current_state)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource = _resource.Get();
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = _current_state;
	barrierDesc.Transition.StateAfter = in_new_state;

	in_command_list->ResourceBarrier(1, &barrierDesc);
	_current_state = in_new_state;

}
