#pragma once

#include "common/ui/ui_enum.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_tint_colour.h"

class IUIComponent;
class ShaderConstantBuffer;
class UIData;
class UIGeometry;
class UIHierarchyNode;
class UIScreenSpace;

struct UIHierarchyNodeUpdateParam;
struct UIManagerDrawParam;

/// To allow for the top level node not to have redundant data, UIHierarchyNode has been split
/// UIHierarchyNodeChild and UIHierarchyNode could be one class, but a UIGeometry for the root node did not make sense
class UIHierarchyNodeChild
{
public:
    //can you pass    std::unique_ptr<IUIComponent>& in_content = nullptr, hmmn, wants non const but i want to use move semantics
    static std::shared_ptr<UIHierarchyNodeChild> Factory(
        UIHierarchyNodeChild* const in_parent_to_this_node_or_null,
        const UILayout& in_layout,
        const UITintColour& in_tint_colour,
        void* in_source_token,
        const UIStateFlag in_state_flag = UIStateFlag::TNone,
        const UIStateDirty in_state_dirty = UIStateDirty::TDefault
        );

    UIHierarchyNodeChild(
        UIHierarchyNodeChild* const in_parent_or_null,
        std::unique_ptr<UIGeometry>& in_geometry,
        std::unique_ptr<IUIComponent>& in_component,
        std::unique_ptr<UIHierarchyNode>& in_node,
        std::unique_ptr<UIScreenSpace>& in_screen_space,
        const UILayout& in_layout,
        const UITintColour& in_tint_colour,
        void* in_source_token = nullptr,
        const UIStateFlag in_state_flag = UIStateFlag::TNone,
        const UIStateDirty in_state_dirty = UIStateDirty::TDefault
        );
    ~UIHierarchyNodeChild();

    /// allows making a map externally of source token to IUIComponent
    void* GetSourceToken() const;

    void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical);
    VectorFloat2& GetUVScrollRef(){ return _uv_scroll; }

    void SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable);
    const bool GetStateFlagBit(const UIStateFlag in_state_flag_bit) const;

    void SetStateDirtyBit(const UIStateDirty in_state_flag_bit, const bool in_enable);
    const bool GetStateDirtyBit(const UIStateDirty in_state_flag_bit) const;


    /// Set layout dirty flag on change, 
    /// WARNING, should SetLayout only be called from UIData, to move things like slider, set layout in UIData?
    void SetLayout(const UILayout& in_layout);
    const UILayout& GetLayout() const { return _layout; }

    void SetTintColour(const UITintColour& in_layout);
    const VectorFloat4 CalculateTintColour() const;

    const float GetTimeAccumulateSeconds() const { return _time_accumulate_seconds; }
    void SetTimeAccumulateSeconds(const float in_time_accumulate_seconds) { _time_accumulate_seconds = in_time_accumulate_seconds; return; }

    /// make sure our component matches what is in the given UIData, set layout, resource, render dirty as required
    void ApplyComponent(
        UIData& in_data,
        const UIHierarchyNodeUpdateParam& in_param,
        const int in_child_index
        );

    /// support recusion by passing through to call UpdateHierarchy on our node
    void UpdateHierarchy(
        UIData& in_data,
        const UIHierarchyNodeUpdateParam& in_param,
        const int in_child_index
        );

    const VectorInt2 GetDesiredSize(
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_parent_window
        );

    //void Draw(const UIManagerDrawParam& in_draw_param);

    //const bool VisitComponents(const std::function<const bool(IUIComponent* const, UIHierarchyNode* const)>& in_visitor);

    UIHierarchyNode& GetNode() const;
    UIScreenSpace& GetScreenSpace() const;

    void Finalise(
        const VectorInt2& in_base_layout_size,
        const VectorInt2& in_base_desired_size,
        const VectorInt2& in_parent_offset
        );

    void UpdateLayout(
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window,
        const VectorInt2& in_parent_offset
        );

    void UpdateResources(
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size
        );

    void Update(
        const float in_time_delta
        );
    void UpdateScroll(
        const UIHierarchyNodeUpdateParam& in_param
        );
    void UpdateGeometry(
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size
        );
    /// Get things ready
    void PreDraw(
        const UIManagerDrawParam& in_draw_param
        );
    /// Set the render target (node ui texture) and draw the child to is, using the children geometry and textures
    void Draw(
        const UIManagerDrawParam& in_draw_param//,
        //const bool in_dirty,
        //const UIStateFlag in_state_flag
        );

private:
    /// recusion point
    std::unique_ptr<UIHierarchyNode> _node;

    /// _component is out here rather than in _node to avoid top level node needing a root or do nothing content for layout
    std::unique_ptr<IUIComponent> _component; 

    /// Need to track if state changed, so UIGeometry tracks pos and uv, rather than just GeometryGeneric
    /// this is the geometry that we use to paint ourself (the texture under our child node) to the texture under the parent node
    std::unique_ptr<UIGeometry> _geometry;
    /// size relative to screen for input, device coords, [-1,-1] ... [1,1] (lower left to upper right)
    std::unique_ptr<UIScreenSpace> _screen_space;

    /// shader constants to allow for tint, shared as it has ownership incremented by use in the render list
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

    //====================================================
    // The following members were previouly in component

    /// We either need to have a copy of the layout from the UIData, 
    ///  or pass around the relevant UIData while doing the layout update
    ///  or store a lot of values like parent size, offset, window?
    /// don't need to override layout if changes for scroll ect are sent to the UIData layout via calbacks?
    /// correction, what have _uv_scroll state and some flags for manual control of scroll, 
    /// auto scroll is not part of the ui data (model) but slider pos is, and component can set 
    UILayout _layout;

    /// what flag should be changed? effects render
    UITintColour _tint_colour;

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

    /// layout and render dirty flags, propergate to parent when set active
    UIStateDirty _state_dirty;

    /// have a link to our parent so changes to the state flag can be up propergated if needed
    UIHierarchyNodeChild* _parent_or_null;

    /// with UpdateLayout, UpdateScroll, Geometry all split up, needed to store some state. could move to param?
    VectorInt2 _layout_size;
    VectorInt2 _layout_offset;

    /// cache the parent sizes so we can know if we need to recalculate on parent size change, ie, window resize. 
    /// alternative is to do this on the top level node and broadcast? otherwise this is more generic at expence of some memory
    VectorInt2 _cache_parent_window;
    VectorInt2 _cache_parent_offset;

};
