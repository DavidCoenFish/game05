#pragma once

class Timer
{
public:
	Timer();
	const float GetDeltaSeconds();
private:
	std::chrono::steady_clock::time_point m_timePoint;

};
