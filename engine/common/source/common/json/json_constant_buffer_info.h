#pragma once
#include "json/json.hpp"

struct ConstantBufferInfo;

// Void to_json(nlohmann::json& j, const ConstantBufferInfo& p);
// Void from_json(const nlohmann::json& j, ConstantBufferInfo& p);
// Void from_json(const nlohmann::json& j, std::vector< std::shared_ptr< ConstantBufferInfo > >& p);
struct JSONConstantBufferInfo
{
public:
    JSONConstantBufferInfo() 
        : _float_count(0)
        , _visiblity(D3D12_SHADER_VISIBILITY_ALL)
    {
        // Nop
    }


public:
    int _float_count;
    D3D12_SHADER_VISIBILITY _visiblity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    JSONConstantBufferInfo,
    in_float_count,
    in_visiblity
    );
const std::vector < std::shared_ptr < ConstantBufferInfo > > TransformConstantBufferInfo(const std::vector < \
    JSONConstantBufferInfo >&in_input);
