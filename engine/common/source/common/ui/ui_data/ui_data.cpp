#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data.h"

#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"


UIData::UIData(
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null,
    UIData* const in_parent_or_null
    )
    : _layout(in_layout)
    , _base_colour(in_base_colour)
    , _state_flag_tint_array_or_null(in_state_flag_tint_array_or_null)
    , _parent_or_null(in_parent_or_null)
{
    // Nop
}

UIData::~UIData()
{
    // Nop
}

// return reference to child, to allow chain
UIData* const UIData::AddChild(const std::shared_ptr<UIData>& in_data_child)
{
    _array_child_data.push_back(in_data_child);
    SetDirtyBit(UIDataDirty::THierarchy, true);
    return in_data_child.get();
}

UIData* const UIData::SetChild(const std::shared_ptr<UIData>& in_data_child, const int in_index)
{
    while (_array_child_data.size() <= in_index)
    {
        _array_child_data.push_back(nullptr);
    }
    _array_child_data[in_index] = in_data_child;
    SetDirtyBit(UIDataDirty::THierarchy, true);
    return in_data_child.get();
}

void UIData::ClearAllChildren(void)
{
    if (0 < _array_child_data.size())
    {
        _array_child_data.clear();
        SetDirtyBit(UIDataDirty::THierarchy, true);
    }
}

/// return true if any bit of flag is set
const bool UIData::GetDirtyBit(const UIDataDirty in_flag) const
{
    return 0 != (static_cast<int>(_dirty_flag) & static_cast<int>(in_flag));
}

/// modifying hierarchy [AddChild, SetChild, ClearAllChildren] automatically sets hierarchy modified flag, 
/// otherwise client code is expexed to set dirty flag appropriatly, ie, if a UIData derrive class changes something that effects render
void UIData::SetDirtyBit(const UIDataDirty in_flag, const bool in_active)
{
    const UIDataDirty prev = _dirty_flag;
    if (true == in_active)
    {
        _dirty_flag = static_cast<UIDataDirty>(static_cast<int>(_dirty_flag) | static_cast<int>(in_flag));
    }
    else
    {
        _dirty_flag = static_cast<UIDataDirty>(static_cast<int>(_dirty_flag) & ~static_cast<int>(in_flag));
    }

    if ((true == in_active) && (prev != _dirty_flag) && (nullptr != _parent_or_null))
    {
        _parent_or_null->SetDirtyBit(in_flag, true);
    }

    return;
}

void UIData::UpdateHierarchy(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateHierarchyParam&, //in_param,
    const int //in_index
    )
{
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentCanvas* content = dynamic_cast<UIComponentCanvas*>(in_out_component.get());
    if (nullptr == content)
    {
        auto new_content = std::make_unique<UIComponentCanvas>();
        in_out_component = std::move(new_content);
        SetDirtyBit(UIDataDirty::TLayout, true);
    }

    SetDirtyBit(UIDataDirty::THierarchy, false);

    return;
}

/// replacing what was done in the factory/ component
void UIData::UpdateLayoutRender(
    IUIComponent& in_component,
    UIHierarchyNodeChildData& in_component_owner,
    const VectorInt2& in_parent_texture_size,
    const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
    const UIScreenSpace& in_parent_screen_space
    )
{
    // need the node so set texture needs to draw?
    VectorInt2 content_size;
    if (true == _layout.UsesContentSize())
    {
        //content_size = GetContentSize(in_texture_size, in_component_owner);
    }
    const VectorInt2 layout_size = _layout.GetSize(
        in_parent_texture_size, 
        in_param._ui_scale,
        content_size
        );

    const bool uv_scroll_manual_x = in_component.GetStateFlagBit(UIStateFlag::TManualScrollX);
    const bool uv_scroll_manual_y = in_component.GetStateFlagBit(UIStateFlag::TManualScrollY);

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;
    IUIComponent::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        in_component.GetUVScrollRef(),
        uv_scroll_manual_x,
        uv_scroll_manual_y,
        in_parent_texture_size,
        VectorInt2(),
        in_parent_texture_size,
        in_param._ui_scale,
        in_param._time_delta,
        layout_size,
        layout_size,
        _layout
        );

    // Update geometry
    if (true == in_component_owner._geometry->Set(
        geometry_pos,
        geometry_uv
        ))
    {
        SetDirtyBit(UIDataDirty::TRender, true);
        // do we need to set the parent texture as dirty? or return true?
    }

    in_component_owner._screen_space->Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    SetDirtyBit(UIDataDirty::TLayoutRender, false);
}
