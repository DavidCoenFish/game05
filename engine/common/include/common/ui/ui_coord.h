#pragma once

class VectorFloat2;
class VectorInt2;

class UICoord
{
public:
    enum class ParentSource
    {
        None,
        X,
        Y,
        Min,
        Max
        // make a 4:3 box in the parent and use the x or y
    };

    explicit UICoord(
        const ParentSource in_parent_source = ParentSource::None,
        const float in_ratio = 0.0f,
        const float in_em_offset = 0.0f
        );
    ~UICoord();

    const float Calculate(
        const VectorFloat2& in_parent_size,
        const float in_ui_scale
        ) const;

    const int Calculate(
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        ) const;

    const bool operator==(const UICoord& in_rhs) const;
    const bool operator!=(const UICoord& in_rhs) const;

private:
    ParentSource _parent_source;
    float _ratio;
    float _em_offset; // pixel offset, but in em for ui scale

};
