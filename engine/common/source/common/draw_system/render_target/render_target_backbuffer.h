#pragma once
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class HeapWrapperItem;

class RenderTargetBackBuffer : public IRenderTarget
{
public:
	RenderTargetBackBuffer(
		DrawSystem* const in_draw_system,
		ID3D12Device* const in_device,
		const int in_buffer_index,
		const RenderTargetFormatData&in_target_format_data,
		const RenderTargetDepthData&in_target_depth_data,
		IDXGISwapChain* const in_swap_chain,
		const VectorInt2& in_size
		);

private:
	virtual void StartRender(ID3D12GraphicsCommandList* const in_command_list, const bool in_allow_clear) override;
	virtual void EndRender(ID3D12GraphicsCommandList* const in_command_list) override;
	virtual void GetFormatData(
		DXGI_FORMAT&in_depth_format,
		int&in_render_target_view_format_count,
		const DXGI_FORMAT*&in_render_target_view_format
		) const override;
	virtual const VectorInt2 GetSize() const override;

private:
	int _buffer_index;
	RenderTargetFormatData _target_format_data;
	D3D12_CLEAR_VALUE _target_clear_value;
	RenderTargetDepthData _target_depth_data;
	D3D12_CLEAR_VALUE _depth_clear_value;
	D3D12_VIEWPORT _screen_viewport;
	D3D12_RECT _scissor_rect;
	VectorInt2 _back_buffer_size;
	Microsoft::WRL::ComPtr < ID3D12Resource > _render_target_resource;
	std::shared_ptr < HeapWrapperItem > _render_target_descriptor;
	Microsoft::WRL::ComPtr < ID3D12Resource > _depth_resource;
	std::shared_ptr < HeapWrapperItem > _depth_stencil_view_descriptor;
	D3D12_RESOURCE_STATES _current_state;
};
