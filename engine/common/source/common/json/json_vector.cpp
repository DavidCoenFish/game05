#include "common/common_pch.h"

#include "common/json/json_vector.h"
#include "common/math/vector_float4.h"

// #define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
// #define NLOHMANN_JSON_FROM(v1) if(nlohmann_json_j.contains(#v1)){ nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); }
void to_json(
    in_nlohmann::json&in_j,
    const VectorFloat4&in_p
    )
{
    in_j = in_nlohmann::json::array(
    {
        in_p[0], in_p[1], in_p[2], in_p[3]}

        ); return;
    }

    void from_json(
        const in_nlohmann::json&in_j,
        VectorFloat4&in_p
        )
    {
        if (in_j.is_array())
        {
            in_p[0] = in_j[0];
            in_p[1] = in_j[1];
            in_p[2] = in_j[2];
            in_p[3] = in_j[3];
        }
        return;
    }

