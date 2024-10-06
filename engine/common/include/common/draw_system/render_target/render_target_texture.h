#pragma once
#include "common/draw_system/i_resource.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class HeapWrapperItem;

class RenderTargetTexture : public IRenderTarget, public IResource
{
public:
	struct Resource
	{
	public:
		explicit Resource(
			const D3D12_CLEAR_VALUE& in_clear_value,
			const DXGI_FORMAT in_format = DXGI_FORMAT_UNKNOWN,
			const Microsoft::WRL::ComPtr<ID3D12Resource>& in_render_target = nullptr,
			const std::shared_ptr<HeapWrapperItem>& in_render_target_view_descriptor = nullptr,
			const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_descriptor = nullptr,
			const bool in_clear_color = false,
			const bool in_clear_depth = false,
			const bool in_clear_stencil = false
			);

	public:
		D3D12_CLEAR_VALUE _clear_value;
		DXGI_FORMAT _format;
		Microsoft::WRL::ComPtr < ID3D12Resource > _render_target;
		std::shared_ptr < HeapWrapperItem > _render_target_view_descriptor;
		std::shared_ptr < HeapWrapperItem > _shader_resource_view_descriptor;
		bool _clear_color;
		bool _clear_depth;
		bool _clear_stencil;
	};
	RenderTargetTexture(
		DrawSystem* const in_draw_system,
		const std::vector < RenderTargetFormatData >&in_target_format_data_array,
		const RenderTargetDepthData&in_target_depth_data,
		const VectorInt2& in_size,
		const bool in_resize_with_screen = false
		);
	virtual ~RenderTargetTexture();
	std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem(const int in_index = 0) const;
	std::shared_ptr<HeapWrapperItem> GetDepthResourceHeapWrapperItem() const;
	std::shared_ptr<HeapWrapperItem> GetDepthShaderResourceHeapWrapperItem() const;
	void Resize(
		ID3D12GraphicsCommandList* const in_command_list,
		ID3D12Device2* const in_device,
		const VectorInt2& in_size
		);

private:
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const in_command_list,
		ID3D12Device2* const in_device
		) override;
	virtual void OnResize(
		ID3D12GraphicsCommandList* const in_command_list,
		ID3D12Device2* const in_device,
		const VectorInt2& in_screen_size
		) override;
	virtual void StartRender(ID3D12GraphicsCommandList* const in_command_list, const bool in_allow_clear) override;
	virtual void EndRender(ID3D12GraphicsCommandList* const in_command_list) override;
	void TransitionResource(
		ID3D12GraphicsCommandList* const in_command_list,
		const D3D12_RESOURCE_STATES in_new_state_render_target,
		const D3D12_RESOURCE_STATES in_new_state_depth_resource
		);
	// D3D12_RESOURCE_STATE_DEPTH_WRITE	= 0x10,
	// D3D12_RESOURCE_STATE_DEPTH_READ	= 0x20,
	// Virtual void GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) const override;
	virtual void GetFormatData(
		DXGI_FORMAT&in_depth_format,
		int&in_render_target_view_format_count,
		const DXGI_FORMAT*&in_render_target_view_format
		) const override;
	virtual const VectorInt2 GetSize() const override;

public:
	std::vector < std::shared_ptr < Resource > > _target_resource_array;
	std::vector < D3D12_CPU_DESCRIPTOR_HANDLE > _array_render_target_descriptors;
	// Cache the cpu handle of the target resource
	std::shared_ptr < Resource > _depth_resource;
	D3D12_VIEWPORT _screen_viewport;
	D3D12_RECT _scissor_rect;
	D3D12_RESOURCE_STATES _current_state_render_target;
	D3D12_RESOURCE_STATES _current_state_depth_resource;

private:
	VectorInt2 _size;
	bool _resize_with_screen;
	std::vector < DXGI_FORMAT > _target_format_array;
	int _id;

};
