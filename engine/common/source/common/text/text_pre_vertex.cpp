#include "common/common_pch.h"
#include "common/text/text_pre_vertex.h"

#include "common/math/vector_int2.h"
#include "common/text/text_cell.h"
#include "common/util/vector_helper.h"

TextPreVertex::TextPreVertex(
	const int in_default_line_height
	)
	: _vertical_bounds(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max())
	, _line_vertical_bounds(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max())
	, _bound_dirty(true)
	, _line_dirty(false)
	, _line_index(0)
	, _default_line_height(in_default_line_height)
	, _current_line_height(0)
{
	_horizontal_bounds.push_back(VectorInt2(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max()));
}

TextPreVertex::~TextPreVertex()
{
	// Nop
}

void TextPreVertex::Reserve(const unsigned int glyph_count)
{
	_pre_vertex_data.reserve(glyph_count + _pre_vertex_data.size());
}

void TextPreVertex::AddPreVertexScale(
	const TextCell& in_cell,
	const int in_pos_x,
	const int in_pos_y,
	const float in_new_line_gap_ratio,
	const int in_colour,
	const float in_ui_scale
	)
{
	const VectorInt2 width_height = in_cell.GetWidthHeight(in_ui_scale);
	const VectorInt2 bearing = in_cell.GetBearing(in_ui_scale);
	const int line_height = static_cast<int>(round(width_height.GetY() * (1.0f + in_new_line_gap_ratio)));
	AddPreVertex(
		width_height,
		bearing,
		in_cell.GetUVRef(),
		in_cell.GetMask(),
		in_pos_x,
		in_pos_y,
		line_height,
		in_colour
		);
}

void TextPreVertex::AddPreVertex(
	const TextCell& in_cell,
	const int in_pos_x,
	const int in_pos_y,
	const int in_line_height,
	const int in_colour
	)
{
	AddPreVertex(
		in_cell.GetWidthHeightRef(),
		in_cell.GetBearingRef(),
		in_cell.GetUVRef(),
		in_cell.GetMask(),
		in_pos_x,
		in_pos_y,
		in_line_height,
		in_colour
		);
}

void TextPreVertex::AddPreVertex(
	const VectorInt2& in_width_height,
	const VectorInt2& in_bearing,
	const VectorFloat4& in_uv,
	const int in_mask,
	const int in_pos_x,
	const int in_pos_y,
	const int in_line_height,
	const int  in_colour
	)
{
	_line_dirty = true;
	_bound_dirty = true;
	_current_line_height = std::max(_current_line_height, in_line_height);

	const int pos_x = in_pos_x + in_bearing.GetX();
	const int pos_y = in_pos_y - (in_width_height.GetY() - in_bearing.GetY());
	const VectorInt4 pos = VectorInt4(
		pos_x,
		pos_y,
		pos_x + in_width_height.GetX(),
		pos_y + in_width_height.GetY()
		);

	_pre_vertex_data.push_back(PreVertexData({
		pos,
		in_uv,
		in_mask,
		in_colour,
		_line_index
		}));

	_line_vertical_bounds[0] = std::min(_line_vertical_bounds[0], pos[1]);
	_line_vertical_bounds[1] = std::max(_line_vertical_bounds[1], pos[3]);

	while ((int)_horizontal_bounds.size() <= _line_index)
	{
		_horizontal_bounds.push_back(VectorInt2(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max()));
	}

	_horizontal_bounds[_line_index][0] = std::min(_horizontal_bounds[_line_index][0], pos[0]);
	_horizontal_bounds[_line_index][1] = std::max(_horizontal_bounds[_line_index][1], pos[2]);

	return;
}

void TextPreVertex::AddCursor(
	const int in_pos_x,
	const int //in_pos_y
	)
{
	// This was added to get fixed width more stable. 
	// possibly some error with GetBounds not correctly using min _horizontal_bounds, thus adding a min(0)
	_horizontal_bounds[_line_index][0] = std::min(_horizontal_bounds[_line_index][0], 0);
	_horizontal_bounds[_line_index][1] = std::max(_horizontal_bounds[_line_index][1], in_pos_x);
}

void TextPreVertex::StartNewLine(
	VectorInt2& in_out_cursor
	)
{
	FinishLine();
	in_out_cursor[0] = 0;
	if (0 != _line_index)
	{
		in_out_cursor[1] -= _current_line_height;
	}

	_line_index += 1;
	_current_line_height = 0;
	_line_vertical_bounds = VectorInt2(std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
}

const VectorInt2 TextPreVertex::GetBounds( const TextEnum::VerticalBlockAlignment in_vertical_block_alignment)
{
	FinishLine();

	if (true == _bound_dirty)
	{
		_bound_dirty = false;
		VectorInt2 horizontal(
			std::numeric_limits<int>::max(),
			-std::numeric_limits<int>::max()
			);
		for (const auto& iter : _horizontal_bounds)
		{
			horizontal[0] = std::min(horizontal[0], iter[0]);
			horizontal[1] = std::max(horizontal[1], iter[1]);
		}
		_bounds[0] = horizontal[1] - horizontal[0]; 
		_bounds[1] = _vertical_bounds[1] - _vertical_bounds[0]; 
	}

	VectorInt2 result = _bounds;
	switch(in_vertical_block_alignment)
	{
	default:
		break;
	case TextEnum::VerticalBlockAlignment::BottomEM:
	case TextEnum::VerticalBlockAlignment::MiddleEM:
	case TextEnum::VerticalBlockAlignment::TopEM:
		result[1] = std::max(_default_line_height, _vertical_bounds[1]) - _vertical_bounds[0];
		result[1] = std::max(result[1], _default_line_height);
		break;
	}

	return result;
}

void TextPreVertex::BuildVertexData(
	std::vector<uint8_t>& out_vertex_raw_data,
	const VectorInt2& in_container_size,
	const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
	const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
	const int in_em_size // Used for alignments MiddleEM, TopEM, BottomEM
	)
{
	FinishLine();

	int vertical_delta = 0;
	const int line_count = static_cast<int>(_horizontal_bounds.size());
	std::vector<int> horizontal_line_delta(line_count);
	for (int index = 0; index < line_count; ++index)
	{
		const int width =  _horizontal_bounds[index][1] -  _horizontal_bounds[index][0];
		switch (in_horizontal_line_alignment)
		{
		default:
			break;
		case TextEnum::HorizontalLineAlignment::Left:
			break;
		case TextEnum::HorizontalLineAlignment::Middle:
			horizontal_line_delta[index] = ((in_container_size[0] - width) / 2) - _horizontal_bounds[index][0];
			break;
		case TextEnum::HorizontalLineAlignment::Right:
			horizontal_line_delta[index] = (in_container_size[0] - _horizontal_bounds[index][1]);
			break;
		}
	}
	switch (in_vertical_block_alignment)
	{
	default:
		break;
	case TextEnum::VerticalBlockAlignment::Bottom:
		// glphys that decend under base line, can result in _vertical_bounds being less than zero
		vertical_delta = -(_vertical_bounds[0]);
		break;
	case TextEnum::VerticalBlockAlignment::BottomEM:
		vertical_delta = in_em_size / 2;
		break;
	case TextEnum::VerticalBlockAlignment::Middle:
		vertical_delta = (((in_container_size[1]) - (_vertical_bounds[1] - _vertical_bounds[0])) / 2) - _vertical_bounds[0];
		break;
	case TextEnum::VerticalBlockAlignment::MiddleEM:
		vertical_delta = ((in_container_size[1] - in_em_size) / 2);
		break;
	case TextEnum::VerticalBlockAlignment::Top:
		vertical_delta = in_container_size[1] - _vertical_bounds[1];
		break;
	case TextEnum::VerticalBlockAlignment::TopEM:
		vertical_delta = in_container_size[1] - in_em_size;
		break;
	}

	for (const auto& item : _pre_vertex_data)
	{
		const int horizontal_delta = horizontal_line_delta[item._line_index];
		const VectorFloat4 pos = VectorFloat4(
			(((item._pos_low_high[0] + horizontal_delta) / static_cast<float>(in_container_size.GetX())) * 2.0f) - 1.0f,
			(((item._pos_low_high[1] + vertical_delta) / static_cast<float>(in_container_size.GetY())) * 2.0f) - 1.0f,
			(((item._pos_low_high[2] + horizontal_delta) / static_cast<float>(in_container_size.GetX())) * 2.0f) - 1.0f,
			(((item._pos_low_high[3] + vertical_delta) / static_cast<float>(in_container_size.GetY())) * 2.0f) - 1.0f
			);

		//0.0f, 0.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);

		//0.0f, 1.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);

		//1.0f, 0.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);

		//1.0f, 0.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);

		//0.0f, 1.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);

		//1.0f, 1.0f,
		VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
		VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
		VectorHelper::AppendValue(out_vertex_raw_data, item._colour);
	}

	return;
}

void TextPreVertex::FinishLine()
{
	if (false == _line_dirty)
	{
		return;
	}

	_line_dirty = false;

	if (0 != _line_index)
	{
		// use the default line height. this could be caused by an empty line. otherwise use max line height from things on line
		if (0 == _current_line_height)
		{
			_current_line_height = _default_line_height;
		}

		//move backward over _pre_vertex_data, items on _line_index
		for (std::vector<PreVertexData>::reverse_iterator i = _pre_vertex_data.rbegin(); 
			i != _pre_vertex_data.rend(); ++i ) 
		{
			PreVertexData& item = *i;
			if (item._line_index != _line_index)
			{
				break;
			}
			item._pos_low_high[1] -= _current_line_height;
			item._pos_low_high[3] -= _current_line_height;
		}

		_line_vertical_bounds[0] -= _current_line_height;
		_line_vertical_bounds[1] -= _current_line_height;
	}

	_vertical_bounds[0] = std::min(_vertical_bounds[0], _line_vertical_bounds[0]);
	_vertical_bounds[1] = std::max(_vertical_bounds[1], _line_vertical_bounds[1]);

	return;
}
