#pragma once

#include "common/math/vector_int4.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/text/text_enum.h"

class TextCell;
class VectorInt2;

/*
    better to make a new TextPreVertex than add a reset?
    could add current_x, current_y to make change of font easer in text_block, but enough in this class?

    to allow accomidate multiple font/size on a line,
    only adjust the vertical pos of the pre vertex on a line once the line is done, so we can use the max height of the line
*/
class TextPreVertex
{
public:
    struct PreVertexData
    {
        VectorInt4 _pos_low_high;
        VectorFloat4 _uv_low_high;
        VectorFloat4 _mask;
        VectorFloat4 _colour;
        int _line_index;
    };

    TextPreVertex(
        const float _default_line_height
        );
    ~TextPreVertex();

    //void Reset();

    void Reserve(const unsigned int glyph_count);

    void AddPreVertex(
        const TextCell* const in_cell,
        const int in_pos_x,
        const int in_pos_y,
        const int _line_height,
        const VectorFloat4& _colour
        );

    void StartNewLine(
        VectorInt2& in_out_cursor
        );

    const VectorInt2 GetBounds();

    void BuildVertexData(
        std::vector<uint8_t>& out_vertex_raw_data,
        const int in_glyph_size, // Used for alignments MiddleEM, TopEM, BottomEM
        const VectorInt2& in_containter_size,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        );

private:
    void FinishLine();

private:
    std::vector<PreVertexData> _pre_vertex_data;
    VectorInt2 _vertical_bounds;
    std::vector<VectorInt2> _horizontal_bounds;

    bool _bound_dirty;
    VectorInt2 _bounds;
    int _line_index;

    bool _line_dirty; // pre vertex added to line, but not adjusted for max height on line
    VectorInt2 _line_vertical_bounds;
    int _default_line_height; // if nothing added to the line, use this height
    int _current_line_height; // allow for things added to the line to be taller

};
