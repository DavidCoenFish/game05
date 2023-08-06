#pragma once

class UICoord
{
public:
    UICoord(
        const float in_ratio,
        const float in_em_offset
        );
    ~UICoord();

    const float Calculate(
        const float in_parent_dim,
        const float in_ui_scale
        ) const;

private:
    float _ratio;
    float _em_offset; // pixel offset, but in em for ui scale

};
