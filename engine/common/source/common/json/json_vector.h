#pragma once
#include "json/json.hpp"

class VectorFloat4;

// NLOHMANN_DEFINE_TYPE_INTRUSIVE(
// VectorFloat4
// );
void to_json(
    nlohmann::json& in_j,
    const VectorFloat4& in_p
    );
void from_json(
    const nlohmann::json&in_j,
    VectorFloat4&in_p
    );
