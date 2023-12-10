#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data.h"

#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"

const std::vector<std::shared_ptr<UIEffectData>> UIData::s_empty_effect_data_array;


UIData::UIData(
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    UIData* const in_parent_or_null
    )
    : _layout(in_layout)
    , _base_colour(in_base_colour)
    , _array_effect_data(in_array_effect_data)
    , _parent_or_null(in_parent_or_null)
    , _dirty_flag(UIDataDirty::TNone)
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
    in_data_child->_parent_or_null = this;
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
    in_data_child->_parent_or_null = this;
    SetDirtyBit(UIDataDirty::THierarchy, true);
    return in_data_child.get();
}

void UIData::ClearAllChildren(void)
{
    if (0 < _array_child_data.size())
    {
        for (auto& item : _array_child_data)
        {
            item->_parent_or_null = nullptr;
        }

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

void UIData::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam&, //in_param,
    const int //in_index
    )
{
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentCanvas* component = dynamic_cast<UIComponentCanvas*>(in_out_component.get());
    if (nullptr == component)
    {
        auto new_component = std::make_unique<UIComponentCanvas>(
            _layout,
            this
            );
        //component = new_component.get();
        in_out_component = std::move(new_component);
    }
    else
    {
        component->SetLayout(_layout);
    }

    return;
}

//const bool UIData::BuildEffectStack(
//    std::vector<std::shared_ptr<UIEffectComponent>>& in_out_array_effect_component,
//    const UIHierarchyNodeApplyParam& in_param
//    )
//{
//    //_array_effect_data
//}

//
///// replacing what was done in the factory/ component
//void UIData::UpdateLayoutRender(
//    IUIComponent& in_component,
//    UIHierarchyNodeChildData& in_component_owner,
//    const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
//    const VectorInt2& in_parent_size,
//    const VectorInt2& in_parent_offset,
//    const VectorInt2& in_parent_window,
//    const UIScreenSpace& in_parent_screen_space
//    )
//{
//    VectorInt2 content_size;
//    if (true == _layout.UsesContentSize())
//    {
//        content_size = GetContentSize(
//            in_component,
//            in_parent_window, 
//            in_param._ui_scale,
//            in_component_owner
//            );
//    }
//
//    VectorInt2 layout_size;
//    VectorInt2 desired_size;
//    if ((true == GetDirtyBit(UIDataDirty::TLayout)) ||
//        (false == in_component.CheckLayoutCache(layout_size, desired_size, in_parent_window, content_size)))
//    {
//        layout_size = _layout.GetSize(
//            in_parent_window, 
//            in_param._ui_scale,
//            content_size
//            );
//        desired_size = GetDesiredSize(
//            in_component,
//            layout_size, 
//            in_param._ui_scale,
//            in_component_owner
//            );
//
//        in_component.SetLayoutCache(layout_size, desired_size, in_parent_window, content_size);
//        SetDirtyBit(UIDataDirty::TLayout, false);
//    }
//
//    const bool uv_scroll_manual_x = in_component.GetStateFlagBit(UIStateFlag::TManualScrollX);
//    const bool uv_scroll_manual_y = in_component.GetStateFlagBit(UIStateFlag::TManualScrollY);
//
//    VectorFloat4 geometry_pos;
//    VectorFloat4 geometry_uv;
//    VectorInt2 texture_size;
//    IUIComponent::CalculateGeometry(
//        geometry_pos,
//        geometry_uv,
//        texture_size,
//        in_component.GetUVScrollRef(),
//        uv_scroll_manual_x,
//        uv_scroll_manual_y,
//        in_parent_size, 
//        in_parent_offset, 
//        in_parent_window,
//        in_param._ui_scale,
//        in_param._time_delta,
//        layout_size,
//        desired_size,
//        _layout
//        );
//
//    // Update geometry
//    if (true == in_component_owner._geometry->Set(
//        geometry_pos,
//        geometry_uv
//        ))
//    {
//        SetDirtyBit(UIDataDirty::TRender, true);
//        // do we need to set the parent texture as dirty? or return true?
//
//        // is this safe, only update screen space if geometry has changed?
//        in_component_owner._screen_space->Update(
//            in_parent_screen_space,
//            geometry_pos,
//            geometry_uv
//            );
//    }
//
//    in_component_owner._node->UpdateLayoutRender(
//        in_param,
//        *this,
//        texture_size,
//        VectorInt2(),
//        texture_size,
//        *(in_component_owner._screen_space)
//        );
//
//    return;
//}
//
//const VectorInt2 UIData::GetContentSize(
//    IUIComponent&, //in_component,
//    const VectorInt2& in_target_size, 
//    const float in_ui_scale,
//    UIHierarchyNodeChildData& in_component_owner
//    )
//{
//    // std::vector<std::shared_ptr<UIData>> _array_child_data;
//    if (0 == _array_child_data.size())
//    {
//        return VectorInt2();
//    }
//    const int target_length = static_cast<int>(_array_child_data.size());
//    DSC_ASSERT(target_length == in_component_owner._node->GetChildData().size(), "we expect size of data children and size child data array to match");
//
//    VectorInt2 result;
//    for (int index = 0; index < target_length; ++index)
//    {
//        UIHierarchyNodeChildData* const child_component_owner = in_component_owner._node->GetChildData()[index].get();
//        UIData* const child_data = _array_child_data[index].get();
//
//        if ((nullptr != child_component_owner) && (nullptr != child_data))
//        {
//            const VectorInt2 child_size = child_data->GetDesiredSize(
//                *child_component_owner->_component,
//                in_target_size,
//                in_ui_scale,
//                *child_component_owner
//                );
//            result = VectorInt2::Max(result, child_size);
//        }
//    }
//    return result;
//}
//
//const VectorInt2 UIData::GetDesiredSize(
//    IUIComponent& in_component,
//    const VectorInt2& in_target_size, 
//    const float in_ui_scale,
//    UIHierarchyNodeChildData& in_component_owner
//    )
//{
//    // would it be better to just use the max size of the children? canvas desired size is the max of the children
//    VectorInt2 content_size = GetContentSize(
//        in_component,
//        in_target_size,
//        in_ui_scale,
//        in_component_owner
//        );
//    if (true == _layout.UsesContentSize())
//    {
//        content_size = _layout.GetSize(content_size, in_ui_scale, content_size);
//    }
//    return content_size;
//}
//
