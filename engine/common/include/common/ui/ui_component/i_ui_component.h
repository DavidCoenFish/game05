#pragma once

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

/// interface base class for the ui components, do not store members here, see UIComponentDefault
class IUIComponent
{
public:
    /// pure interface
    IUIComponent();
    /// virtual dtor as this class is used to store derrived types
    virtual ~IUIComponent();

    /// Allow slider to modify a component layout
    virtual const UILayout& GetLayout() const = 0; 

    /// Allow slider to override the layout
    virtual void SetLayoutOverride(const UILayout& in_override) = 0; 
    virtual void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical) = 0;

    virtual const bool SetStateFlag(const UIStateFlag in_state_flag) = 0;
    virtual const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable) = 0;
    virtual const UIStateFlag GetStateFlag() const = 0;

    /// Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) = 0;
    /// Make sorting children easier
    virtual void* GetSourceToken() const = 0;

    /// make the hirearchy match the model (UIData)
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_parent_child_data, // the parent/ owner of the current component
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) = 0;

    /// convert the layout data and parent size to the texture size, geometry size and uv
    /// certain component types may do slightly different operations, but the default is to call GetDesiredSize
    /// needed to return true as scroll under manual scroll would be modified without the data flagging dirty, as easier to catch geometry, but geometry change can only mark texture dirty via update size
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
        ) = 0;

    /// certain layout data allows shrink, and certain componets may have different logic, such as stack component
    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) = 0;

    /// prep the children of the component for draw
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) = 0;
};
