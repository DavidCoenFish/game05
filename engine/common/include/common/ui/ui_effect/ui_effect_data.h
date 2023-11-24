#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_coord.h"

class UIEffectComponent;

class UIEffectData
{
public:
    typedef std::array<VectorFloat4, static_cast<int>(UIStateFlag::TTintPermutationCount)> TStateFlagTintArray;

    /// let the text manager what locale hint to render text as, for harfbuz
    UIEffectData(
        const UIEffectEnum in_type = UIEffectEnum::TNone,
        const UICoord& in_coord_a = UICoord(),
        const UICoord& in_coord_b = UICoord(),
        const UICoord& in_coord_c = UICoord(),
        const UICoord& in_coord_d = UICoord(),
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null = nullptr
        );
    virtual ~UIEffectData();

    void ApplyComponent(
        std::shared_ptr<UIEffectComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param,
        const int in_index = 0
        );

private:
    /// either we don't use type none as default ui shader, or need to be carefull with type to use with _shader_constant_buffer
    UIEffectEnum _type;

    /// used to build the shader constants
    UICoord _coord_a;
    /// used to build the shader constants
    UICoord _coord_b;
    /// used to build the shader constants
    UICoord _coord_c;
    /// used to build the shader constants
    UICoord _coord_d;

    /// array of tint colours to be selected from
    std::shared_ptr<const TStateFlagTintArray> _state_flag_tint_array_or_null;

};
