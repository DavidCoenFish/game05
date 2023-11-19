#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_coord.h"

class IUIComponent;

class UIDataEffect : public UIData
{
    typedef UIData TSuper;
public:
    /// let the text manager what locale hint to render text as, for harfbuz
    UIDataEffect(
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null = nullptr,
        const UIEffectEnum in_type = UIEffectEnum::TNone,
        const UICoord& in_coord_a = UICoord(),
        const UICoord& in_coord_b = UICoord(),
        const UICoord& in_coord_c = UICoord(),
        const UICoord& in_coord_d = UICoord()
        );
    virtual ~UIDataEffect();

private:
    /// Make component type match what the data wants, default is UIComponentCanvas
    virtual void UpdateHierarchy(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const int in_index = 0
        ) override;

    virtual void UpdateLayoutRender(
        IUIComponent& in_component,
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const UIScreenSpace& in_parent_screen_space
        ) override;

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

};
