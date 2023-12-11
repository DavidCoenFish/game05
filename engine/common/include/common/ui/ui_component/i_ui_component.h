#pragma once

#include "common/ui/ui_enum.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UIData;
class Shader;
class UIEffectComponent;
class UIGeometry;
class UIHierarchyNode;
class UILayout;
class UIScreenSpace;
class UITexture;
class VectorInt2;
class VectorFloat2;
class VectorFloat4;

struct UIManagerDrawData;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeUpdateParam;

enum class UIStateFlag;

/// interface base class for the ui components, previously had a UIComponentDefault for members and implmentation
/// while it was cute to not inherit implmementation, the amount of traffic copy virtual functions to
/// add accessors to interface was becoming problematic, so merging UIComponentDefault into IUIComponent
/// some data has been moved back into UIData [layout,base colour, tint array]
/// and bring back tint array into IUIComponent to try and issolate UIData from input and render (admitably this is a rather arbitaty seperation)
class IUIComponent
{
public:
    IUIComponent(
        const UILayout& in_layout,
        void* in_source_token = nullptr,
        const UIStateFlag in_state_flag = UIStateFlag::TNone
        );

    /// virtual dtor as this class is used to store derrived types
    virtual ~IUIComponent();

    static void UpdateScroll(
        VectorFloat2& in_out_scroll,
        const bool in_scroll_manual_x,
        const bool in_scroll_manual_y,
        const float in_time_delta,
        const VectorInt2& in_layout_size,
        const VectorInt2& in_texture_size
        );

    static void CalculateGeometry(
        VectorFloat4& out_geometry_pos,
        VectorFloat4& out_geometry_uv,
        const VectorInt2& in_parent_texture_size,
        const VectorInt2& in_layout_offset,
        const VectorInt2& in_layout_size,
        const VectorInt2& in_texture_size,
        const VectorFloat2& in_scroll,
        const UILayout& in_layout 
        );

    void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical);
    VectorFloat2& GetUVScrollRef(){ return _uv_scroll; }

    /// return true bits changed
    const bool SetStateFlag(const UIStateFlag in_state_flag);
    const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable);
    const bool GetStateFlagBit(const UIStateFlag in_state_flag_bit) const;

    /// Set layout dirty flag on change
    void SetLayout(const UILayout& in_layout);
    const UILayout& GetLayout() const { return _layout; }

    /// Make sorting children easier
    void SetSourceToken(void* in_source_ui_data_token);
    /// allows making a map externally of source token to IUIComponent
    void* GetSourceToken() const;

    const float GetTimeAccumulateSeconds() const { return _time_accumulate_seconds; }
    void SetTimeAccumulateSeconds(const float in_time_accumulate_seconds) { _time_accumulate_seconds = in_time_accumulate_seconds; return; }

    //const bool CheckLayoutCache(
    //    VectorInt2& out_layout_size, 
    //    VectorInt2& out_desired_size, 
    //    const VectorInt2& in_parent_window
    //    );
    //void SetLayoutCache(
    //    const VectorInt2& in_layout_size, 
    //    const VectorInt2& in_desired_size, 
    //    const VectorInt2& in_parent_window
    //    );

    //void ApplyEffect(
    //    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    //    const UIHierarchyNodeUpdateParam& in_param,
    //    const int in_index
    //    );


    /// For text, the textblock size. 
    /// Do we need an assert that text with width limit can not be child of a layout with shrink? do not want in_parent_window to change during update
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_pre_shrink_layout_size //in_parent_window
        );

    virtual void UpdateLayout(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window
        );

    virtual void UpdateResources(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size
        );

    /// prep the children of the component for draw. 
    /// for example, component string may use this to run the TextBock::Draw, setting up the components' nodes' texture for the component to be drawn to it's parent latter in the Node::Draw
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        );

protected:
    /// an internal update to save have a lot of code copied around, the tick may dirty the tint colour, ie, trying to do a fade
    const bool Update(
        const float in_time_delta
        );

private:
    // moved to effect
    //std::shared_ptr<const TStateFlagTintArray> _state_flag_array_or_null;

    /// We either need to have a copy of the layout from the UIData, 
    ///  or pass around the relevant UIData while doing the layout update
    ///  or store a lot of values like parent size, offset, window?
    /// don't need to override layout if changes for scroll ect are sent to the UIData layout via calbacks?
    UILayout _layout;

    /// uv = abs(_uv_scroll), and use range [-1...1] wrapped when advancing _uv_scroll, to allow saw tooth animation
    /// this is not in layout to simplify comparison, ie, this may change, but don't want uv animation to dirty layout
    /// Do want the geometry uv change to dirty render however
    VectorFloat2 _uv_scroll;

    /// keep track of lifespan/ duration of the component
    float _time_accumulate_seconds;

    /// maps to UIData, but we don't hold onto reference other than treating it as an ID/ token
    void* _source_token;

    /// still have need of hidden and disabled? plus input state [hover, touch, action]
    UIStateFlag _state_flag;

    VectorInt2 _layout_size;
    VectorInt2 _layout_offset;

    /// values for a layout cache, change to hash? [parent size, offse, window, content size]?
    //VectorInt2 _cache_layout_size;
    //VectorInt2 _cache_desired_size; 
    //VectorInt2 _cache_parent_window; 

    //std::vector<std::shared_ptr<UIEffectComponent>> _effect_component_array;

};
