#include "pch.h"

#include "ID.h"

const int ID::GetID()
{
	static int s_trace = 0;
	return ++s_trace;
}
