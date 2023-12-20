#pragma once

#include "common/ui/ui_enum.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_tint_colour.h"

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
class UIHierarchyNodeChild;
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

    /// For text, the textblock size. 
    /// Do we need an assert that text with width limit can not be child of a layout with shrink? do not want in_parent_window to change during update
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_pre_shrink_layout_size //in_parent_window
        );

    virtual void UpdateLayout(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window,
        const VectorInt2& in_parent_offset
        );

    virtual void UpdateResources(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size
        );

    /// prep the children of the component for draw. 
    /// for example, component string may use this to run the TextBock::Draw, setting up the components' nodes' texture for the component to be drawn to it's parent latter in the Node::Draw
    virtual void PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        );

protected:
    /// an internal update to save have a lot of code copied around, the tick may dirty the tint colour, ie, trying to do a fade
    //const bool Update(
    //    const float in_time_delta
    //    );


};
