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
    , _state_flag_tint_array_or_null(in_state_flag_tint_array_or_null)
{
    // Nop
}

UIEffectData::~UIEffectData()
{
    // Nop
}

const bool UIEffectData::ApplyComponent(
    std::shared_ptr<UIEffectComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam&, // in_param,
    const int //in_index
    )
{
    bool dirty = false;
    UIEffectComponent* component = dynamic_cast<UIEffectComponent*>(in_out_component.get());
    if (nullptr == component)
    {
        dirty = true;
        in_out_component = std::make_shared<UIEffectComponent>(
            _type,
            _coord_a,
            _coord_b,
            _coord_c,
            _coord_d,
            _state_flag_tint_array_or_null
            );
    }
    else
    {
        if (true == component->Set(
            _type,
            _coord_a,
            _coord_b,
            _coord_c,
            _coord_d,
            _state_flag_tint_array_or_null
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}
