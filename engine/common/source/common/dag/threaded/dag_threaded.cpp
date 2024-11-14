#include "common/common_pch.h"
#include "common/dag/threaded/dag_threaded.h"

template <>
const bool DagThreaded::IsZero<std::string>(const std::string& value)
{
	return value.empty();
}
