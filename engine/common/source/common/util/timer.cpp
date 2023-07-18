#include "common/common_pch.h"

#include "common/util/timer.h"

Timer::Timer()
	: _time_point(std::chrono::high_resolution_clock::now())
{
	return;
}

const float Timer::GetDeltaSeconds()
{
	const auto now = std::chrono::high_resolution_clock::now();
	const float delta = std::chrono::duration<float, std::ratio<1, 1>>(now - _time_point).count();
	_time_point = now;
	return delta;
}
