// Issue. Have paranoia about having all windows includes across translation units have the same defines/settings
// Solution. Have the windows include in a common file, common.h

#pragma once


#include <random>
#include <memory>

#include <json/json.hpp>

// the idea with the namespace name is to be able to replace static lib, so change the lib path on new major version $(SolutionDir)static_lq_00, 01, 02
// but the internal use in code is still via a "static_lq" path and a "static_lq" namespace
namespace static_lq
{
}

