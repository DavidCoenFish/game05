#include "static_lq/static_lq_pch.h"
#include "static_lq/name/i_name_system_generator.h"

const std::string StaticLq::INameSystemGenerator::GenerateKey(const std::string& in_generator_key, const int in_seed)
{
    return std::string("slqns_") + in_generator_key + std::to_string(in_seed);
}
