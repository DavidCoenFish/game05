#pragma once

#include "common/ui/ui_enum.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UIData;
class Shader;
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
struct UIHierarchyNodeLayoutData;
struct UIHierarchyNodeUpdateHierarchyParam;

enum class UIStateFlag;

/// interface base class for the ui components, previously had a UIComponentDefault for members and implmentation
/// while it was cute to not inherit implmementation, the amount of traffic copy virtual functions to
/// add accessors to interface was becoming problematic, so merging UIComponentDefault into IUIComponent
/// some data has been moved back into UIData [layout,base colour, tint array]
class IUIComponent
{
public:
    /// pure interface
    IUIComponent(
        void* in_source_token = nullptr,
        const UIStateFlag in_state_flag = UIStateFlag::TNone
        );
    /// virtual dtor as this class is used to store derrived types
    virtual ~IUIComponent();

    static void CalculateGeometry(
        VectorFloat4& out_geometry_pos,
        VectorFloat4& out_geometry_uv,
        VectorInt2& out_texture_size,
        VectorFloat2& in_out_scroll,
        const bool in_uv_scroll_manual_x,
        const bool in_uv_scroll_manual_y,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        const VectorInt2& in_layout_size, // layout size != in_parent_window, unless layout size is 100% of the parent window...
        const VectorInt2& in_desired_size,
        const UILayout& in_layout 
        );

    void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical);
    VectorFloat2& GetUVScrollRef(){ return _uv_scroll; }

    /// return true bits changed
    const bool SetStateFlag(const UIStateFlag in_state_flag);
    const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable);
    const bool GetStateFlagBit(const UIStateFlag in_state_flag_bit) const;

    /// Make sorting children easier
    void SetSourceToken(void* in_source_ui_data_token);
    /// Make sorting children easier
    void* GetSourceToken() const;

    const float GetTimeAccumulateSeconds() const { return _time_accumulate_seconds; }
    void SetTimeAccumulateSeconds(const float in_time_accumulate_seconds) { _time_accumulate_seconds = in_time_accumulate_seconds; return; }

    const bool CheckLayoutCache(
        VectorInt2& out_layout_size, 
        VectorInt2& out_desired_size, 
        const VectorInt2& in_parent_size, 
        const VectorInt2& in_parent_offset, 
        const VectorInt2& in_parent_window, 
        const VectorInt2& in_content_size
        );
    void SetLayoutCache(
        const VectorInt2& in_layout_size, 
        const VectorInt2& in_desired_size, 
        const VectorInt2& in_parent_size, 
        const VectorInt2& in_parent_offset, 
        const VectorInt2& in_parent_window, 
        const VectorInt2& in_content_size
        );


public:
    ///// make the hirearchy match the model (UIData)
    //virtual const bool UpdateHierarchy(
    //    UIData* const in_data,
    //    UIHierarchyNodeChildData& in_out_parent_child_data, // the parent/ owner of the current component
    //    const UIHierarchyNodeUpdateHierarchyParam& in_param
    //    );

    ///// convert the layout data and parent size to the texture size, geometry size and uv
    ///// certain component types may do slightly different operations, but the default is to call GetDesiredSize
    ///// needed to return true as scroll under manual scroll would be modified without the data flagging dirty, as easier to catch geometry, but geometry change can only mark texture dirty via update size
    //virtual const bool UpdateSize(
    //    DrawSystem* const in_draw_system,
    //    const VectorInt2& in_parent_size,
    //    const VectorInt2& in_parent_offset,
    //    const VectorInt2& in_parent_window,
    //    const float in_ui_scale,
    //    const float in_time_delta, 
    //    UIGeometry& in_out_geometry, 
    //    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text,
    //    const UIScreenSpace& in_parent_screen_space,
    //    UIScreenSpace& out_screen_space
    //    );

    ///// certain layout data allows shrink, and certain componets may have different logic, such as stack component
    //virtual void GetDesiredSize(
    //    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    //    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    //    const VectorInt2& in_parent_window,
    //    const float in_ui_scale,
    //    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    //    );

    /// prep the children of the component for draw
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        );

    /// allow the UIHierarchicalNode::Draw to access the tint colour, some components my use it in their custom PreDraw so return white here
    //virtual const VectorFloat4 GetTintColour() const;

protected:
    /// an internal update to save have a lot of code copied around, the tick may dirty the tint colour, ie, trying to do a fade
    const bool Update(
        const float in_time_delta
        );

private:
    /// uv = abs(_uv_scroll), and use range [-1...1] wrapped when advancing _uv_scroll, to allow saw tooth animation
    /// this is not in layout to simplify comparison, ie, this may change, but don't want uv animation to dirty layout
    VectorFloat2 _uv_scroll;

    /// keep track of lifespan/ duration of the component
    float _time_accumulate_seconds;

    /// maps to UIData, but we don't hold onto reference other than treating it as an ID/ token
    void* _source_token;

    /// still have need of hidden and disabled?
    UIStateFlag _state_flag;

    /// values for a layout cache, change to hash [parent size, offse, window, content size]?
    VectorInt2 _layout_size;
    VectorInt2 _desired_size; 
    VectorInt2 _parent_size;
    VectorInt2 _parent_offset; 
    VectorInt2 _parent_window; 
    VectorInt2 _content_size;

};
