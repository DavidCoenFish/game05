#include "common/common_pch.h"
#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_effect/ui_effect_data.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_hierarchy_node_child.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_screen_space.h"

namespace
{
    const float CalculatePos(const int in_pos, const int in_size)
    {
        return ((static_cast<float>(in_pos) / static_cast<float>(in_size)) * 2.0f) - 1.0f;
    }

    constexpr float s_scroll_speed = 5.0f;
}

IUIComponent::IUIComponent()
{
    // Nop
}

IUIComponent::~IUIComponent()
{
    // Nop
}

void IUIComponent::UpdateScroll(
    VectorFloat2& in_out_scroll,
    const bool in_scroll_manual_x,
    const bool in_scroll_manual_y,
    const float in_time_delta,
    const VectorInt2& in_layout_size,
    const VectorInt2& in_texture_size
    )
{
    const VectorInt2 delta(
        in_texture_size[0] - in_layout_size[0],
        in_texture_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        if (false == in_scroll_manual_x)
        {
            const float delta_f = static_cast<float>(delta[0]);
            float new_value = in_out_scroll[0] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[0] = new_value;
        }
    }

    if (0 < delta[1])
    {
        if (false == in_scroll_manual_y)
        {
            const float delta_f = static_cast<float>(delta[1]);
            float new_value = in_out_scroll[1] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[1] = new_value;
        }
    }

    return;
}

void IUIComponent::CalculateGeometry(
    VectorFloat4& out_geometry_pos,
    VectorFloat4& out_geometry_uv,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset,
    const VectorInt2&,// in_layout_offset,
    const VectorInt2& in_layout_size,
    const VectorInt2& in_texture_size,
    const VectorFloat2& in_scroll,
    const UILayout& in_layout 
    )
{
    const VectorInt2 window_attach = in_layout.GetParentWindowAttach(in_parent_window, in_parent_offset); 
    const VectorInt2 layout_pivot = in_layout.GetLayoutPivot(in_layout_size);

    // Deal pos
    //VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f)
    out_geometry_pos = VectorFloat4(
        CalculatePos(window_attach[0] - layout_pivot[0], in_parent_size[0]),
        CalculatePos(window_attach[1] - layout_pivot[1], in_parent_size[1]),
        CalculatePos(window_attach[0] - layout_pivot[0] + in_layout_size[0], in_parent_size[0]),
        CalculatePos(window_attach[1] - layout_pivot[1] + in_layout_size[1], in_parent_size[1])
        );

    out_geometry_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f);
    const VectorInt2 delta(
        in_texture_size[0] - in_layout_size[0],
        in_texture_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        const float length = (static_cast<float>(in_layout_size[0]) / static_cast<float>(in_texture_size[0]));
        const float offset = (1.0f - length) * abs(in_scroll[0]);
        out_geometry_uv[0] = offset;
        out_geometry_uv[2] = offset + length;
    }

    if (0 < delta[1])
    {
        const float length = (static_cast<float>(in_layout_size[1]) / static_cast<float>(in_texture_size[1]));
        const float offset = (1.0f - length) * abs(in_scroll[1]);
        out_geometry_uv[1] = offset + length;
        out_geometry_uv[3] = offset;
    }

    return;
}

const VectorInt2 IUIComponent::GetDesiredSize(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& in_parent_window
    )
{
#if 1
    VectorInt2 base_layout_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_layout_param._ui_scale);
    return base_layout_size;
    //return in_component_owner.GetLayout().AddMargin(
    //    base_layout_size,
    //    in_layout_param._ui_scale
    //    );
#else
    VectorInt2 max_child_size;
    // want something indicitive of size for text with width limit
    const VectorInt2 window_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_layout_param._ui_scale);

    for (auto child: in_component_owner.GetNode().GetChildData())
    {
        max_child_size = VectorInt2::Max(
            max_child_size,
            child->GetDesiredSize(
                in_layout_param,
                window_size
                )
            );
    }

    return in_component_owner.GetLayout().ApplyMargin(
        max_child_size,
        in_layout_param._ui_scale
        );
#endif
}

/// default behavoiur, ie, canvas
/// node::update layout (in parent window)
///     for each child A0
///         component::update layout
///             calculate layout size given parent window
///             recurse node::update layout
///             component::desired(parent window or layout size? layout size may yet to be modified...)
///             finialise layout size (shrink/expand)
void IUIComponent::UpdateLayout(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window
    )
{
    // calculate layout size given parent window
    const VectorInt2 base_layout_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_param._ui_scale);
    const VectorInt2 base_layout_minus_margin = in_component_owner.GetLayout().SubtractMargin(base_layout_size, in_param._ui_scale);

    // offset for texture margin, stack needs to add
    const VectorInt2 base_offset = in_component_owner.GetLayout().GetLayoutOffset(in_param._ui_scale);

    // recurse node::update layout. default (ie, canvas behaviour) is for children to have no offset from us
    // to have children with different layouts, we override this method, ie, UIComponentStack
    in_component_owner.GetNode().UpdateLayout(
        in_param,
        base_layout_minus_margin,
        base_offset //VectorInt2::s_zero  //base_offset
        );

    // component::desired(parent window or layout size? layout size may yet to be modified...), add texture margin
    const VectorInt2 base_desired_size = GetDesiredSize(in_component_owner, in_param, base_layout_size);

    // finialise layout size (shrink/expand) and work out the texture size (which may include the texture margin)
    in_component_owner.Finalise(
        base_layout_size,
        base_desired_size
        );

    return;
}

void IUIComponent::UpdateResources(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space,
    const VectorInt2& in_parent_texture_size,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset,
    const VectorInt4& in_texture_margin
    )
{
    in_component_owner.Update(in_param._delta_time_seconds);
    in_component_owner.UpdateScroll(in_param);
    in_component_owner.UpdateGeometry(
        in_param, 
        in_parent_screen_space,
        in_parent_texture_size,
        in_parent_window,
        in_parent_offset
        );

    in_component_owner.GetNode().UpdateResources(
        in_param,
        in_component_owner.GetScreenSpace(),
        in_texture_margin
        );

    return;
}

void IUIComponent::PreDraw(
    const UIManagerDrawParam&, // in_draw_param,
    UIHierarchyNode& //in_node
    )
{
    //in_node.PreDraw(in_draw_param);
    return;
}
