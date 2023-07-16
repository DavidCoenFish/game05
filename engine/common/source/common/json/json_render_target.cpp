#include "common/common_pch.h"

#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"
#include "common/json/json_draw_system_enum.h"
#include "common/json/json_render_target.h"

// #define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
// #define NLOHMANN_JSON_FROM(v1) if(nlohmann_json_j.contains(#v1)){ nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); }
void to_json(
    in_nlohmann::json&in_j,
    const RenderTargetFormatData&in_p
    )
{
    in_j["format"] = in_p._format;
    in_j["clearColor"] = in_p._clear_color;
    in_j["clearDepthOnSet"] = in_p._clear_on_set;
}

void from_json(
    const in_nlohmann::json&in_j,
    RenderTargetFormatData&in_p
    )
{
    if (in_j.contains("format"))
    {
        in_j.at("format") .get_to(in_p._format);
    }
    if (in_j.contains("clearColor"))
    {
        in_j.at("clearColor") .get_to(in_p._clear_color);
        if (in_j.contains("clearOnSet"))
        {
            in_j.at("clearOnSet") .get_to(in_p._clear_on_set);
        }
        else
        {
            in_p._clear_on_set = true;
        }
    }
}

void to_json(
    in_nlohmann::json&in_j,
    const RenderTargetDepthData&in_p
    )
{
    in_j["format"] = in_p._format;
    in_j["clearDepthOnSet"] = in_p._clear_depth_on_set;
    in_j["clearDepth"] = in_p._clear_depth;
    in_j["clearStencilOnSet"] = in_p._clear_stencil_on_set;
    in_j["clearStencil"] = in_p._clear_stencil;
    in_j["shaderResource"] = in_p._shader_resource;
}

void from_json(
    const in_nlohmann::json&in_j,
    RenderTargetDepthData&in_p
    )
{
    if (in_j.contains("format"))
    {
        in_j.at("format") .get_to(in_p._format);
    }
    if (in_j.contains("clearDepth"))
    {
        in_j.at("clearDepth") .get_to(in_p._clear_depth);
        if (in_j.contains("clearDepthOnSet"))
        {
            in_j.at("clearDepthOnSet") .get_to(in_p._clear_depth_on_set);
        }
        else
        {
            in_p._clear_depth_on_set = true;
        }
    }
    if (in_j.contains("clearStencil"))
    {
        in_j.at("clearStencil") .get_to(in_p._clear_stencil);
        if (in_j.contains("clearStencilOnSet"))
        {
            in_j.at("clearStencilOnSet") .get_to(in_p._clear_stencil_on_set);
        }
        else
        {
            in_p._clear_stencil_on_set = true;
        }
    }
    if (in_j.contains("shaderResource"))
    {
        in_j.at("shaderResource") .get_to(in_p._shader_resource);
    }
    return;
}

