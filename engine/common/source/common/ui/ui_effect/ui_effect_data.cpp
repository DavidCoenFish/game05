#include "common/common_pch.h"
#include "common/ui/ui_effect/ui_effect_data.h"

#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"

UIEffectData::UIEffectData(
    const UIEffectEnum in_type,
    const UICoord& in_coord_a,
    const UICoord& in_coord_b,
    const UICoord& in_coord_c,
    const UICoord& in_coord_d,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null
    )
    : _type(in_type)
    , _coord_a(in_coord_a)
    , _coord_b(in_coord_b)
    , _coord_c(in_coord_c)
    , _coord_d(in_coord_d)
{
    // Nop
}

UIEffectData::~UIEffectData()
{
    // Nop
}

void UIEffectData::UpdateHierarchy(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateHierarchyParam&, //in_param,
    const int// in_index
    )
{
    bool dirty = false;
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentEffect* component = dynamic_cast<UIComponentEffect*>(in_out_component.get());
    if (nullptr == component)
    {
        auto new_component = std::make_unique<UIComponentEffect>(
            _type,
            _coord_a,
            _coord_b,
            _coord_c,
            _coord_d,
            GetStateFlagTintArray()
            );
        component = new_component.get();
        in_out_component = std::move(new_component);
        SetDirtyBit(UIDataDirty::TLayout, true);
    }
    else
    {
        if (true == component->Set(
            _type,
            _coord_a,
            _coord_b,
            _coord_c,
            _coord_d,
            GetStateFlagTintArray()
            ))
        {
            dirty = true;
        }
    }

    if (true == dirty)
    {
        SetDirtyBit(UIDataDirty::TLayout, true);
    }

    SetDirtyBit(UIDataDirty::THierarchy, false);

    return;
}

void UIEffectData::UpdateLayoutRender(
    IUIComponent& in_component,
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const UIScreenSpace& in_parent_screen_space
    )
{
    TSuper::UpdateLayoutRender(
        in_component,
        in_component_owner,
        in_param,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_parent_screen_space
        );

    UIComponentEffect* component = dynamic_cast<UIComponentEffect*>(&in_component);
    if (nullptr != component)
    {
        const VectorInt2 texture_size = in_component_owner._node->GetTextureSize(
            in_param._draw_system
            );
        component->SetContainerSize(
            texture_size,
            in_param
            );
    }
}
