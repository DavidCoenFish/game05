#pragma once

#include "common/ui/ui_component/i_ui_component.h"


class UIComponentScroll;

/*

UIHierarchyNode // N0 
    _texture // T0 texture or backbuffer A0 draws to
    [_input_state] // I0 optional top level input state, which node is focused, node click started on, hover node
    _child_node_array // A0
        _geometry // G1 geometry to draw the texture T1 onto T0
        _shader_constant_buffer // S1 the shader constants -> moved to component default? or needs to be with? in? geometry G1
        _component // C1 controls size of T1 and G1. model returns an array of ui data for child array A1
        UIHierarchyNode // N1 child node
            _texture // T1 texture or backbuffer A1 draws to
            _child_node_array // A1
                _geometry // G2 geometry to draw the texture T1 onto T0
                _shader_constant_buffer // S2 the shader constants -> moved to component default? or needs to be with? in? geometry G1
                _component // C2 controls size of T1 and G1. model returns an array of ui data for child array A1
                UIHierarchyNode // N2 child node
                    _texture // T2 texture or backbuffer A1 draws to
                    _child_node_array // A2

    
    C1 is a manual scroll component
    C2 is some arbitary child content
    we control the scrolling of T1 for the manual scroll
*/
class UIComponentManualScroll : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    /// _child_data_array[0], some arbitary child content that may or may not need to be scrolled to fit layout size
    /// _child_data_array[1], top level node of horizontal scroll stack, or null if we don't allow horizontal scroll
    /// _child_data_array[2], top level node of vertical scroll stack, or null if we don't allow vertical scroll
    UIComponentManualScroll(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentManualScroll();

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual const bool UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        ) override;

private:
    IUIComponent* _horizontal_scroll_wrapper;
    IUIComponent* _vertical_scroll_wrapper;

    UIComponentScroll* _horizontal_scroll;
    UIComponentScroll* _vertical_scroll;

};
