#pragma once

class VectorFloat2;

class UICoord
{
public:
    enum class ParentSource
    {
        X,
        Y,
        Min,
        Max
    };

    explicit UICoord(
        const ParentSource in_parent_source,
        const float in_ratio = 0.0f,
        const float in_em_offset = 0.0f
        );
    ~UICoord();

    const float Calculate(
        const VectorFloat2& in_parent_size,
        //const VectorInt2& in_parent_size,
        const float in_ui_scale
        ) const;

private:
    ParentSource _parent_source;
    float _ratio;
    float _em_offset; // pixel offset, but in em for ui scale

};
