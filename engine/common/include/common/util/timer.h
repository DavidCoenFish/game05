#pragma once

class Timer
{
public:
    Timer();

    const float GetDeltaSeconds(float* const out_average_or_nullptr = nullptr);

private:
    std::chrono::steady_clock::time_point _time_point;
    float _delta_seconds_average;

};
