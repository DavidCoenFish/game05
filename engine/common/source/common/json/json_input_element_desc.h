#pragma once
#include "json/json.hpp"

// Void to_json(nlohmann::json& j, const std::vector<D3D12_INPUT_ELEMENT_DESC>& p);
void from_json(
    const in_nlohmann::json&in_j,
    std::vector < D3D12_INPUT_ELEMENT_DESC >&in_p
    );
// Std::vector<D3D12_INPUT_ELEMENT_DESC> JSONInputElementDesc_from_json(
// Const nlohmann::json& json
// );
