#include "common/common_pch.h"

#include "common/util/id.h"

const int ID::GetID()
{
	static int s_trace = 0;
	return ++s_trace;
}
