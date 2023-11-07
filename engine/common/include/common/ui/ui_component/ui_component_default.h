#pragma once

#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_enum.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIComponent;
class UIData;
class Shader;
class ShaderConstantBuffer;
class UIGeometry;
class UIScreenSpace;
class UITexture;
class UIHierarchyNode;
class UIScreenSpace;
class VectorInt2;
class VectorFloat4;

struct UIManagerDrawData;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeLayoutData;
struct UIHierarchyNodeUpdateHierarchyParam;

/// don't inherit functionality, but we can share methods/ components
/// default implementation
class UIComponentDefault
{
public:
    UIComponentDefault(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        void* in_source_token = nullptr,
        const UIStateFlag in_state_flag = UIStateFlag::TNone
        );
    ~UIComponentDefault();

    static void CalculateGeometry(
        VectorFloat4& out_geometry_pos,
        VectorFloat4& out_geometry_uv,
        VectorInt2& out_texture_size,
        VectorFloat2& in_out_scroll,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        const VectorInt2& in_layout_size, // layout size != in_parent_window, unless layout size is 100% of the parent window...
        const VectorInt2& in_desired_size,
        const UILayout& in_layout 
        );

    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout
        );

    void SetSourceToken(void* in_source_ui_data_token);
    void* GetSourceToken() const;

   // const bool SetLayout(const UILayout& in_layout);
    const UILayout& GetLayout() const { return _layout; }

    VectorFloat2& GetUVScroll() { return _uv_scroll; }

    /// Return true if bits under _state_flag_dirty_mask change. 
    /// Is a direct setter too raw, move to enable/disable bit rather than entire flag.
    const bool SetStateFlag(const UIStateFlag in_state_flag);
    const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable);
    const UIStateFlag GetStateFlag() const { return _state_flag; }

    void SetStateFlagDirty(const UIStateFlag in_state_flag_dirty_mask) { _state_flag_dirty_mask = in_state_flag_dirty_mask; return; }

    const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        );

    void UpdateSize(
        DrawSystem* const in_draw_system,
        IUIComponent& in_out_ui_component,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space,
        UILayout* const in_layout_override
        );

    void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        UILayout* const in_layout_override
        );

    const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        );

    const VectorFloat4 GetTintColour(const VectorFloat4* const in_override_tint = nullptr) const;

private:
    /// colour data, like the clear colour, tint colour, fade profile
    UIBaseColour _base_colour;

    /// layout data of the component
    UILayout _layout;

    /// uv = abs(_uv_scroll), and use range [-1...1] wrapped when advancing _uv_scroll, to allow saw tooth animation
    VectorFloat2 _uv_scroll;

    /// keep track of lifespan/ duration of the component
    float _time_accumulate_seconds;

    /// maps to UIData, but we don't hold onto reference other than treating it as an ID/ token
    void* _source_token;

    /// input hover, touch, selected, disabled....
    UIStateFlag _state_flag;
    /// there are bit when if changed, return dirty
    UIStateFlag _state_flag_dirty_mask;

};
