#include "common/common_pch.h"
#include "common/text/text_pre_vertex.h"

#include "common/math/vector_int2.h"
#include "common/text/text_cell.h"
#include "common/util/vector_helper.h"

TextPreVertex::TextPreVertex(
    const float _default_line_height
    )
    : _vertical_bounds(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
    , _line_vertical_bounds(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
    , _bound_dirty(true)
    , _line_dirty(false)
    , _line_index(0)
    , _default_line_height(_default_line_height)
    , _current_line_height(0.0f)
{
    _horizontal_bounds.push_back(VectorFloat2(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()));
}

TextPreVertex::~TextPreVertex()
{
    // Nop
}

void TextPreVertex::Reserve(const unsigned int glyph_count)
{
    _pre_vertex_data.reserve(glyph_count + _pre_vertex_data.size());
}

void TextPreVertex::AddPreVertex(
    const TextCell* const in_cell,
    const float in_pos_x,
    const float in_pos_y,
    const float _line_height,
    const VectorFloat4& _colour
    )
{
    _line_dirty = true;
    _bound_dirty = true;
    _current_line_height = std::max(_current_line_height, _line_height);

    const VectorInt2 width_height = in_cell->GetWidthHeight();
    const VectorInt2 bearing = in_cell->GetBearing();
    const VectorFloat4 uv = in_cell->GetUV();
    const VectorFloat4 mask = in_cell->GetMask();

    const float pos_x = in_pos_x + bearing.GetX();
    const float pos_y = in_pos_y - (width_height.GetY() - bearing.GetY());
    const VectorFloat4 pos = VectorFloat4(
        pos_x,
        pos_y,
        pos_x + width_height.GetX(),
        pos_y + width_height.GetY()
        );

    _pre_vertex_data.push_back(PreVertexData({
        pos,
        uv,
        mask,
        _colour,
        _line_index
        }));

    _line_vertical_bounds[0] = std::min(_line_vertical_bounds[0], pos[1]);
    _line_vertical_bounds[1] = std::max(_line_vertical_bounds[1], pos[3]);

    while ((int)_horizontal_bounds.size() <= _line_index)
    {
        _horizontal_bounds.push_back(VectorFloat2(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()));
    }

    _horizontal_bounds[_line_index][0] = std::min(_horizontal_bounds[_line_index][0], pos[0]);
    _horizontal_bounds[_line_index][1] = std::max(_horizontal_bounds[_line_index][1], pos[2]);

    return;
}

void TextPreVertex::StartNewLine(
    VectorFloat2& in_out_cursor
    )
{
    FinishLine();
    in_out_cursor[0] = 0.0f;
    in_out_cursor[1] -= _current_line_height;

    _line_index += 1;
    _current_line_height = 0.0f;
    _line_vertical_bounds = VectorFloat2(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
}

const VectorInt2 TextPreVertex::GetBounds()
{
    FinishLine();

    if (true == _bound_dirty)
    {
        _bound_dirty = false;
        VectorFloat2 horizontal(
            std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max()
            );
        for (const auto& iter : _horizontal_bounds)
        {
            horizontal[0] = std::min(horizontal[0], iter[0]);
            horizontal[1] = std::max(horizontal[1], iter[1]);
        }
        _bounds[0] = horizontal[1] - horizontal[0]; 
        _bounds[1] = _vertical_bounds[1] - _vertical_bounds[0]; 
    }

    return VectorInt2(
        int(ceil(_bounds[0])),
        int(ceil(_bounds[1]))
        );
}

void TextPreVertex::BuildVertexData(
    std::vector<uint8_t>& out_vertex_raw_data,
    const int in_glyph_size,
    const VectorInt2& in_containter_size,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    FinishLine();

    float vertical_delta = 0.0f;
    const int line_count = (int)_horizontal_bounds.size();
    std::vector<float> horizontal_line_delta(line_count);
    for (int index = 0; index < line_count; ++index)
    {
        switch (in_horizontal_line_alignment)
        {
        default:
            break;
        case TextEnum::HorizontalLineAlignment::Left:
            break;
        case TextEnum::HorizontalLineAlignment::Middle:
            horizontal_line_delta[index] = (in_containter_size[0] - _horizontal_bounds[index][1]) * 0.5f;
            break;
        case TextEnum::HorizontalLineAlignment::Right:
            horizontal_line_delta[index] = (in_containter_size[0] - _horizontal_bounds[index][1]);
            break;
        }
    }
    switch (in_vertical_block_alignment)
    {
    default:
        break;
    case TextEnum::VerticalBlockAlignment::Bottom:
        break;
    case TextEnum::VerticalBlockAlignment::BottomEM:
        break;
    case TextEnum::VerticalBlockAlignment::Middle:
        vertical_delta = ((in_containter_size[1] + in_containter_size[3]) - (_vertical_bounds[0] + _vertical_bounds[1])) * 0.5f;
        break;
    case TextEnum::VerticalBlockAlignment::MiddleEM:
        {
            float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
            vertical_delta = in_containter_size[1] - (temp * 0.5f);
        }
        break;
    case TextEnum::VerticalBlockAlignment::Top:
        vertical_delta = in_containter_size[1] - _vertical_bounds[1];
        break;
    case TextEnum::VerticalBlockAlignment::TopEM:
        {
            float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
            vertical_delta = in_containter_size[1] - temp;
        }
    break;
    }

    for (const auto& item : _pre_vertex_data)
    {
        const float horizontal_delta = horizontal_line_delta[item._line_index];
        const VectorFloat4 pos = VectorFloat4(
            (((item._pos_low_high[0] + horizontal_delta) / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
            (((item._pos_low_high[1] + vertical_delta) / (float)in_containter_size.GetY()) * 2.0f) - 1.0f,
            (((item._pos_low_high[2] + horizontal_delta) / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
            (((item._pos_low_high[3] + vertical_delta) / (float)in_containter_size.GetY()) * 2.0f) - 1.0f
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
    // use the default line height. this could be caused by an empty line. otherwise use max line height from things on line
    if (0.0f == _current_line_height)
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

    _vertical_bounds[0] = std::min(_vertical_bounds[0], _line_vertical_bounds[0]);
    _vertical_bounds[1] = std::max(_vertical_bounds[1], _line_vertical_bounds[1]);

    return;
}
