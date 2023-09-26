#include "common/common_pch.h"

#include "common/util/timer.h"

Timer::Timer()
    : _time_point(std::chrono::high_resolution_clock::now())
    , _delta_seconds_average(0.0f)
    , _nice_fps(0.0f)
    , _nice_fps_time_accumulate(0.0f)
{
    return;
}

const float Timer::GetDeltaSeconds(float* const out_nice_fps_or_nullptr)
{
    const auto now = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration<float, std::ratio<1, 1>>(now - _time_point).count();
    _delta_seconds_average = (0.25f * delta) + (0.75f * _delta_seconds_average);
    _nice_fps_time_accumulate -= delta;
    if (_nice_fps_time_accumulate < 0.0f)
    {
        _nice_fps_time_accumulate += 0.1f;
        _nice_fps = _delta_seconds_average ? 1.0f / _delta_seconds_average : 0.0f;
    }

    if (nullptr != out_nice_fps_or_nullptr)
    {
        (*out_nice_fps_or_nullptr) = _nice_fps;
    }
    _time_point = now;
    return delta;
}
