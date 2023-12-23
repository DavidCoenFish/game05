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
#ifdef _DEBUG
    const std::string& in_debug_name,
#endif
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const UITintColour& in_tint_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    UIData* const in_parent_or_null
    )
    : _layout(in_layout)
    , _base_colour(in_base_colour)
    , _tint_colour(in_tint_colour)
    , _array_effect_data(in_array_effect_data)
    , _parent_or_null(in_parent_or_null)
    , _dirty_flag(UIDataDirty::TCtor)
#ifdef _DEBUG
    , _debug_name(in_debug_name)
#endif

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
    SetDirtyBit(UIDataDirty::TAdd, true);
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
    SetDirtyBit(UIDataDirty::TAdd, true);
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
        SetDirtyBit(UIDataDirty::THierarchyDirty, true);
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

    // when setting a flag true, mark all the parent nodes as decendant dirty
    if ((true == in_active) && (prev != _dirty_flag) && (nullptr != _parent_or_null))
    {
        _parent_or_null->SetDirtyBit(UIDataDirty::TDecendantDirty, true);
    }

    return;
}

const bool UIData::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam&, //in_param,
    const int //in_index
    )
{
#if 0
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentCanvas* component = dynamic_cast<UIComponentCanvas*>(in_out_component.get());
    if (nullptr == component)
    {
        auto new_component = std::make_unique<UIComponentCanvas>(
            _layout,
            _tint_colour,
            this
            );
        //component = new_component.get();
        in_out_component = std::move(new_component);
    }
    else
    {
        component->Set(_layout, _tint_colour);
    }
#endif
    //TODO: add UIDataCanvas to make UIComponentCanvas, and have UIData make a null component

    bool dirty = false;
    if (in_out_component != nullptr)
    {
        in_out_component = nullptr;
        dirty = true;
    }

    return dirty;
}
