#pragma once

#include "common/math/vector_float4.h"

/// A ui elements tint colour, tints the contents and can be used for fade
/// for string component, use for text colour?
class UITintColour
{
public:
    /// Default is white tint
    static const UITintColour FactoryDefault();
    static const UITintColour FactoryBlack();
    static const UITintColour FactoryFade(
        const float in_fade_duration_seconds = 0.0f,
        const float in_delay_per_create_index_seconds = 0.0f,
        const int in_creation_index = 0
        );

    explicit UITintColour(
        const VectorFloat4& in_tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
        const float in_fade_start_seconds = 0.0f,
        const float in_fade_end_seconds = 0.0f
        );

    const bool GetTimeChangeDirty(const float in_pre_time, const float in_new_time) const;

    const VectorFloat4 GetTintColour(
        const float in_time_accumulate_seconds
        ) const;


    const bool operator==(const UITintColour& in_rhs) const;
    const bool operator!=(const UITintColour& in_rhs) const;

private:
    VectorFloat4 _tint_colour;
    float _fade_start_seconds;
    float _fade_end_seconds;

};

