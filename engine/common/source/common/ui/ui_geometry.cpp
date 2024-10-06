#include "common/common_pch.h"
#include "common/ui/ui_geometry.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/log/log.h"
#include "common/ui/ui_manager.h"
#include "common/util/vector_helper.h"

namespace
{
	static const std::vector<D3D12_INPUT_ELEMENT_DESC> s_input_element_desc_array({
		D3D12_INPUT_ELEMENT_DESC
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
		},
		D3D12_INPUT_ELEMENT_DESC
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
		}
	});

	void GeneratedVertexData(
		std::vector<uint8_t>& out_vertex_raw_data,
		const VectorFloat4 in_pos,
		const VectorFloat4 in_uv
		)
	{
		//0.0f, 0.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[1]);

		//0.0f, 1.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[3]);

		//1.0f, 0.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[1]);

		//1.0f, 1.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_pos[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, in_uv[3]);

		return;
	}
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& UIGeometry::GetInputElementDescArray()
{
	return s_input_element_desc_array;
}

void UIGeometry::BuildGeometryData(
	std::vector<uint8_t>& out_vertex_data,
	const VectorFloat4& in_pos,
	const VectorFloat4& in_uv
	)
{
	GeneratedVertexData(
		out_vertex_data,
		in_pos,
		in_uv
		);
	return;
}

UIGeometry::UIGeometry(
	const VectorFloat4& in_pos,
	const VectorFloat4& in_uv
	)
	: _pos(in_pos)
	, _uv(in_uv)
	, _data_changed(true)
{
	// Nop
}

UIGeometry::~UIGeometry()
{
	// Nop
}

void UIGeometry::Get(
	VectorFloat4& out_pos,
	VectorFloat4& out_uv
	) const
{
	out_pos = _pos;
	out_uv = _uv;
}

const bool UIGeometry::Set(
	const VectorFloat4& in_pos,
	const VectorFloat4& in_uv
	)
{
	if ((_pos == in_pos) &&
		(_uv == in_uv))
	{
		return false;
	}

	//LOG_CONSOLE("UIGeometry Change before pos:[%f %f %f %f] uv:[%f %f %f %f]", _pos[0], _pos[1], _pos[2], _pos[3],
	//	_uv[0], _uv[1], _uv[2], _uv[3]);

	_data_changed = true;
	_pos = in_pos;
	_uv = in_uv;

	//LOG_CONSOLE("UIGeometry Change after pos:[%f %f %f %f] uv:[%f %f %f %f]", _pos[0], _pos[1], _pos[2], _pos[3],
	//	_uv[0], _uv[1], _uv[2], _uv[3]);

	return true;
}

void UIGeometry::UpdateGeometry(
	DrawSystem* const in_draw_system,
	ID3D12GraphicsCommandList* const in_command_list
	)
{
	if (nullptr == _geometry)
	{
		std::vector<uint8_t> vertex_data_raw;
		GeneratedVertexData(
			vertex_data_raw,
			_pos,
			_uv
			);

		_geometry = in_draw_system->MakeGeometryGeneric(
			in_command_list,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			s_input_element_desc_array,
			vertex_data_raw,
			4
			);
	}
	else if (true == _data_changed)
	{
		std::vector<uint8_t> vertex_data_raw;
		GeneratedVertexData(
			vertex_data_raw,
			_pos,
			_uv
			);

		in_draw_system->UpdateGeometryGeneric(
			in_command_list,
			_geometry.get(),
			vertex_data_raw
			);
	}

	_data_changed = false;

	return;
}

void UIGeometry::Draw(
	DrawSystem* const in_draw_system,
	DrawSystemFrame* const in_frame
	)
{
	UpdateGeometry(in_draw_system, in_frame->GetCommandList());
	if (nullptr == _geometry)
	{
		return;
	}

	in_frame->Draw(
		_geometry
		);

	return;
}

