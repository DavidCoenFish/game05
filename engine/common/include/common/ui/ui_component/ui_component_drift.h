#pragma once

#include "common/ui/ui_component/i_ui_component.h"


class VectorInt4;
enum class UISlideDirection;

/// slide in content, and allow reverse trigger
/// rename to UIComponentDrift as slide was too close to slider
class UIComponentDrift : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentDrift(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UISlideDirection in_direction,
        const float in_duration
        );
    virtual ~UIComponentDrift();

    const bool SetModelOther(
        const UISlideDirection in_direction,
        const float in_duration
        );

    //if slide in is playing or finished, start sliding out from current pos
    void TriggerSlideOut();
    //if slide out is playing or completed a slide out, start slidding in
    void TriggerSlideIn();

private:
    /// convert the layout data and parent size to the texture size, geometry size and uv
    /// certain component types may do slightly different operations, but the default is to call GetDesiredSize
    virtual const bool UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text,
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        ) override;

private:
    bool _reverse;
    UISlideDirection _direction;
    float _duration;

};
