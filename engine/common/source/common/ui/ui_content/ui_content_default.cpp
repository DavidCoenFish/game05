#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_default.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_manager.h"

#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"

namespace
{
    const float CalculatePos(const int in_pos, const int in_size)
    {
        return ((static_cast<float>(in_pos) / static_cast<float>(in_size)) * 2.0f) - 1.0f;
    }

    constexpr float s_scroll_speed = 25.0f;
}

UIContentDefault::UIContentDefault(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    void* in_source_token
    )
    : _clear_background(in_clear_background)
    , _clear_colour(in_clear_colour)
    , _layout(in_layout)
    , _source_token(in_source_token)
{
    // Nop
}

UIContentDefault::~UIContentDefault()
{
    // Nop
}

void UIContentDefault::CalculateGeometry(
    VectorFloat4& out_geometry_pos,
    VectorFloat4& out_geometry_uv,
    VectorInt2& out_texture_size,
    VectorFloat2& in_out_scroll,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    const VectorInt2& in_layout_size, // layout size != in_parent_window, unless layout size is 100% of the parent window...
    const VectorInt2& in_desired_size,
    const UILayout& in_layout 
    )
{
    const VectorInt2 pivot = in_layout.GetPivot(in_parent_window, in_ui_scale) + in_parent_offset; 
    const VectorInt2 attach = in_layout.GetAttach(in_layout_size, in_ui_scale);

    // Deal pos
    //VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f)
    out_geometry_pos = VectorFloat4(
        CalculatePos(pivot[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] - attach[1], in_parent_size[1]),
        CalculatePos(pivot[0] + in_layout_size[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] + in_layout_size[1] - attach[1], in_parent_size[1])
        );

    // Deal uv
    out_geometry_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f);
    const VectorInt2 delta(
        in_desired_size[0] - in_layout_size[0],
        in_desired_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        const float delta_f = static_cast<float>(delta[0]);
        float new_value = in_out_scroll[0] + (s_scroll_speed * in_time_delta / delta_f);
        new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
        in_out_scroll[0] = new_value;

        const float length = (static_cast<float>(in_parent_window[0]) / static_cast<float>(in_desired_size[0]));//
        const float offset = (1.0f - length) * abs(new_value);
        out_geometry_uv[0] = offset;
        out_geometry_uv[2] = offset + length;
    }

    if (0 < delta[1])
    {
        const float delta_f = static_cast<float>(delta[1]);
        float new_value = in_out_scroll[1] + (s_scroll_speed * in_time_delta / delta_f);
        new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
        in_out_scroll[1] = new_value;

        const float length = (static_cast<float>(in_parent_window[1]) / static_cast<float>(in_desired_size[1]));//
        const float offset = (1.0f - length) * abs(new_value);
        out_geometry_uv[1] = 1.0f - offset;
        out_geometry_uv[3] = 1.0f - (offset + length);
    }

    // Deal texture
    out_texture_size = VectorInt2::Max(in_layout_size, in_desired_size);

    return;
}

const bool UIContentDefault::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    bool dirty = false;

    if (_clear_background != in_clear_background)
    {
        dirty = true;
        _clear_background = in_clear_background;
    }
    if (_clear_colour != in_clear_colour)
    {
        //change of background colour when background is not active doesn't dirty content
        if (true == _clear_background)
        {
            dirty = true;
        }
        _clear_colour = in_clear_colour;
    }

    if (_layout != in_layout)
    {
        dirty = true;
        _layout == in_layout;
        _uv_scroll = VectorFloat2();
    }

    return dirty;
}

// Make sorting children easier
void UIContentDefault::SetSourceToken(void* in_source_ui_data_token)
{
    _source_token = in_source_ui_data_token;
}

void* UIContentDefault::GetSourceToken() const
{
    return _source_token;
}

const bool UIContentDefault::SetLayout(const UILayout& in_layout)
{
    bool dirty = false;
    if (_layout != in_layout)
    {
        _layout = in_layout;
    }
    return dirty;
}

const bool UIContentDefault::UpdateHierarchy(
    IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataContainer* const data_container = dynamic_cast<const UIDataContainer*>(in_data);
    if (nullptr != data_container)
    {
        if (true == data_container->VisitDataArray([
            &in_out_child_data,
            &in_param,
            this
            ](const std::vector<std::shared_ptr<IUIData>>& in_array_data)->bool{
            return in_out_child_data._node->UpdateHierarchy(
                in_param,
                &in_array_data,
                true,
                false,
                _clear_background,
                _clear_colour
                );
        }))
        {
            dirty =true;
        }
    }
    else
    {
        if (true == in_out_child_data._node->UpdateHierarchy(
            in_param,
            nullptr,
            true,
            false,
            _clear_background,
            _clear_colour
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

void UIContentDefault::UpdateSize(
    DrawSystem* const in_draw_system,
    IUIContent& in_out_ui_content,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    VectorInt2 layout_size;
    VectorInt2 desired_size;
    in_out_ui_content.GetDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        _uv_scroll,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta,
        layout_size,
        desired_size,
        _layout
        );

    // Update geometry
    bool dirty = false;
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    // Recurse
    in_out_node.UpdateSize(
        in_draw_system,
        texture_size, // by default, we use the generated size and scroll if required. a stack control may modify this
        VectorInt2(),
        texture_size,
        in_ui_scale,
        in_time_delta,
        dirty,
        out_screen_space
        );

    return;
}

void UIContentDefault::GetDesiredSize(
    VectorInt2& out_layout_size,
    VectorInt2& out_desired_size,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    VectorInt2 max_desired_size;

    out_layout_size = _layout.GetSize(in_parent_window, in_ui_scale);

    // Default is to go through children and see if anyone needs a bigger size than what we calculate
    for (auto iter: in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *iter;
        if (nullptr == child_data._content)
        {
            continue;
        }
        VectorInt2 child_layout_size;
        VectorInt2 child_desired_size;
        child_data._content->GetDesiredSize(
            child_layout_size,
            child_desired_size,
            out_layout_size, 
            in_ui_scale, 
            *child_data._node
            );
        max_desired_size = VectorInt2::Max(max_desired_size, child_desired_size);
    }

    out_layout_size = _layout.CalculateShrinkSize(out_layout_size, max_desired_size);
    out_desired_size = VectorInt2::Max(out_layout_size, max_desired_size);

    return;
}
