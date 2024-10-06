#pragma once
struct RenderTargetDepthData
{
public:
	explicit RenderTargetDepthData(
		const DXGI_FORMAT in_format = DXGI_FORMAT_UNKNOWN,
		const bool in_clear_depth_on_set = false,
		const float in_clear_depth = 1.0f,
		const bool in_clear_stencil_on_set = false,
		const UINT8 in_clear_stencil = 0,
		const bool in_shader_resource = false
		);
	D3D12_CLEAR_VALUE MakeClearValue() const;

public:
	DXGI_FORMAT _format;
	bool _clear_depth_on_set;
	float _clear_depth;
	bool _clear_stencil_on_set;
	UINT8 _clear_stencil;
	bool _shader_resource;
};
