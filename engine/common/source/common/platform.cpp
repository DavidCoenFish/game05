#include "common/common_pch.h"

#include "common/platform.h"

const int Platform::GetCurrentThreadID()
{
	return static_cast<int>(::GetCurrentThreadId());
}

