#pragma once

class TextPreVertex;
class VectorInt2;

class ITextRunData
{
public:
    virtual void BuildPreVertexData(
        TextPreVertex& in_out_pre_vertex_data,
        VectorInt2& in_out_cursor,
        const bool in_width_limit_enabled,
        const int in_width_limit
        ) const = 0;
};