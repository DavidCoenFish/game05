#pragma once

class UICoord
{
public:
    explicit UICoord(
        const float in_ratio = 0.0f,
        const float in_em_offset = 0.0f
        );
    ~UICoord();

    const float Calculate(
        const float in_parent_dim,
        const float in_ui_scale // pixels per em
        ) const;

private:
    float _ratio;
    float _em_offset; // pixel offset, but in em for ui scale

};
