#include "pch.h"
#include "Timer.h"

Timer::Timer()
	: m_timePoint(std::chrono::high_resolution_clock::now())
{
	return;
}

const float Timer::GetDeltaSeconds()
{
	const auto now = std::chrono::high_resolution_clock::now();
	const float delta = std::chrono::duration<float, std::ratio<1, 1>>(now - m_timePoint).count();
	m_timePoint = now;
	return delta;
}
