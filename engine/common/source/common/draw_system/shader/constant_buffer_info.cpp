#include "common/common_pch.h"

#include "common/draw_system/shader/constant_buffer_info.h"

ConstantBufferInfo::ConstantBufferInfo(
	const std::vector<uint8_t>& in_data,
	const D3D12_SHADER_VISIBILITY in_visiblity
	) 
	: _visiblity(in_visiblity)
	, _data(in_data)
{
	// Nop
}
