#include "common/common_pch.h"

#include "common/util/timer.h"

Timer::Timer()
    : _time_point(std::chrono::high_resolution_clock::now())
    , _delta_seconds_average(0.0f)
{
    return;
}

const float Timer::GetDeltaSeconds(float* const out_average_or_nullptr)
{
    const auto now = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration<float, std::ratio<1, 1>>(now - _time_point).count();
    _delta_seconds_average = (0.25f * delta) + (0.75f * _delta_seconds_average);
    if (nullptr != out_average_or_nullptr)
    {
        (*out_average_or_nullptr) = _delta_seconds_average;
    }
    _time_point = now;
    return delta;
}
