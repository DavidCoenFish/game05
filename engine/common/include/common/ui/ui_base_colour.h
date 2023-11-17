#pragma once

#include "common/math/vector_float4.h"

class UIBaseColour
{
public:
    static const UIBaseColour FactoryRoot(const bool in_always_dirty);
    static const UIBaseColour FactoryRedBackground();
    static const UIBaseColour FactoryGreenBackground();
    static const UIBaseColour FactoryBlueBackground();

    explicit UIBaseColour(
        const VectorFloat4& in_clear_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
        const bool in_clear_background = true,
        const VectorFloat4& in_tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
        const bool in_draw_to_texture = true,
        const bool in_always_dirty = false,
        //const float in_fade_duration_seconds = 0.0f,
        //const float in_delay_per_create_index_seconds = 0.0f
        const float in_fade_start_seconds = 0.0f,
        const float in_fade_end_seconds = 0.0f
        );

    const VectorFloat4 GetTintColour(
        const float in_time_accumulate_seconds,
        const VectorFloat4* const in_override_tint
        ) const;

    const bool GetTimeChangeDirty(const float in_pre_time, const float in_new_time) const;

    const VectorFloat4& GetClearColourRef() const { return _clear_colour; }
    const bool GetClearBackground() const { return _clear_background; }

    const bool GetDrawToTexture() const { return _draw_to_texture; }
    const bool GetAlwaysDirty() const { return _always_dirty; }

    const bool operator==(const UIBaseColour& in_rhs) const;
    const bool operator!=(const UIBaseColour& in_rhs) const;

private:
    VectorFloat4 _clear_colour;
    bool _clear_background;
    bool _draw_to_texture;
    bool _always_dirty;
    VectorFloat4 _tint_colour;

    // start and end or duration and delay per create index?
    //float _fade_duration_seconds;
    //float _delay_per_create_index_seconds;
    float _fade_start_seconds;
    float _fade_end_seconds;

};

