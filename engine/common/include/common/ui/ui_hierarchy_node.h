#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UIData;
class UIGeometry;
class UIHierarchyNode;
class UIRootInputState;
class UITexture;
class UILayout;
class UIScreenSpace;
class UIEffectComponent;
class UIManager;
class IUIComponent;
class IUIModel;
class LocaleSystem;
class Shader;
class ShaderConstantBuffer;
class TextManager;

struct UIComponentFactoryParam;
struct UIDataTextRunStyle;
struct UIHierarchyNodeUpdateHierarchyParam;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;

enum class UIStateFlag;

/*
UIHierarchyNode // N0 
    _texture // T0 texture or backbuffer A0 draws to
    [_input_state] // I0 optional top level input state, which node is focused, node click started on, hover node
    _child_node_array // A0
    _effect_stack // E0
        _geometry // G1 geometry to draw the texture T1 onto T0
        _shader_constant_buffer // S1 the shader constants -> moved to component default? or needs to be with? in? geometry G1
        _component // C1 controls size of T1 and G1. model returns an array of ui data for child array A1
        UIHierarchyNode // N1 child node
            _texture // T1 texture or backbuffer A1 draws to
            _child_node_array // A1
            _effect_stack // E1

the component C1 may have special rules (ie, a virtual method) to control how child array A1 is drawn to T1


lets talk about layout.
text may have desired size, ie, height needed for text after wrap, and/or some text bounds
want to wrap a textblock in margin, and possibly other effects like rounded corners (example: tooltip)
want to wrap something wrapping a textblock with another margin and apply effects (example: tooltip with dropshadow)
stack may have n children, and the children can extend on a certain axis
manual scroll may be allow to extend in any direction, but only uses a window to view into
in certain cases, like tooltips, want the layout size to shrink to the desired size, possibly with margin
effects just want to match the childs desired size, having the effect be the parent matches the render hierarchy, but is this a missed opourunity
what if effects where a different array/stack of operations that are performed on a node/component/texture
effect could have it's own texture, in lockstep to node's texture size
effect is the only think that wants a tint array? also move fade to an effect?

this sounds like a pair of [layout size, desired size] 
where the layout size is the size it draws to the parents texture, 
the desired size is the full render size of the child, and there are some scoll uv values for what of the desired size full render is draw to the parent vial layout size

updateHierachy
updateSize // work out layout size, desired size. on the way back up the node hierarchy, apply the layout shrink
updateRenderResources // with the finialised layout/desired size, set up the texture, geometry sizes, also the screenspace data for input
                      // merge updateRenderResources with updateSize?
dealInput
render //was draw, but is draw too close to deal?

component:getDesiredSize(parent window, ok to expand x, ok to expand y)

//calculate the [layout,desired] size
node:updateSize(parent size, offset, window, ok to expand x, ok to expand y)
    component:updateSize
        ...recurse to node:updateSize, internal to component to allow stack and grid to do things with offset and window

example case A, C1 wants to be a text block with a margin, with text wrap of 1/3 of the screen? C1 is a child of root N0
example case B, C1 is a margin around C2 which is a textblock with margin and text wrap of 1/3 screen. C1 is a child of root N0
example case C, C1 is a stack component, with child C2, which is another stack control, with child textblock with wrap of C2 width

textblock component has a getDesiredSize
pass down last non shring parent size?
or is textblock widthlimit given the top level window size? or just uses a UICoord?
UILayout::GetSize(in_parent_size, in_ui_scale, in_top_level_size/in_last_non_shrink_size)

UpdateSize then UpdateLayout? had been thinking of them paired. layout is how much space you have, desired is how much space you want

UILayout gains shrink to content?



*/


/// As UIHierarchyNodeChildData keeps gaining functions, it really should be promotted to a class and moved to it's own file?
struct UIHierarchyNodeChildData
{
    //can you pass    std::unique_ptr<IUIComponent>& in_content = nullptr, hmmn, wants non const but i want to use move semantics
    static std::shared_ptr<UIHierarchyNodeChildData> Factory();

    UIHierarchyNodeChildData(
        std::unique_ptr<UIGeometry>& in_geometry,
        std::unique_ptr<IUIComponent>& in_component,
        std::unique_ptr<UIHierarchyNode>& in_node,
        std::unique_ptr<UIScreenSpace>& in_screen_space
        );
    ~UIHierarchyNodeChildData();

    //const bool RecurseSetStateFlagInput(const UIStateFlag in_state_flag);
    //static const bool RecurseSetStateFlagInput(UIHierarchyNodeChildData* const in_data, const UIStateFlag in_state_flag);

    //void UpdateHierarchy(
    //    UIData* const in_data,
    //    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    //    const int in_index = 0
    //    );

    void Draw(const UIManagerDrawParam& in_draw_param);

    const bool VisitComponents(const std::function<const bool(IUIComponent* const, UIHierarchyNode* const)>& in_visitor);

    /// Need to track if state changed, so not using GeometryGeneric
    std::unique_ptr<UIGeometry> _geometry;
    /// _component is out here rather than in _node to avoid top level node needing a root or do nothing content for layout
    std::unique_ptr<IUIComponent> _component; 
    /// recusion point
    std::unique_ptr<UIHierarchyNode> _node;
    /// size relative to screen for input
    std::unique_ptr<UIScreenSpace> _screen_space;
    /// shader constants to allow for tint, shared as it has ownership incremented by use in the render list
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};

struct UIHierarchyNodeUpdateParam
{
    explicit UIHierarchyNodeUpdateParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        UIManager* const in_ui_manager = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const UIDataTextRunStyle* const in_default_text_style = nullptr,
        const float in_ui_scale = 1.0f
        );
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    UIManager* const _ui_manager;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    const UIDataTextRunStyle* const _default_text_style;
    float _ui_scale;

};


class UIHierarchyNode
{
public:
    UIHierarchyNode();
    ~UIHierarchyNode();

    UIRootInputState& GetOrMakeRootInputState();

    /// nullptr if _bUseBackBuffer is true
    //std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        std::unique_ptr<IUIComponent>& in_content
        );
    const bool ClearChildren();

    /// Expose child data array to allow ui_component to specialise how hieararchy builds
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& GetChildData() { return _child_data_array; }

    /// create/ destroy nodes to match model, make content match type from factory, update content?
    void UpdateHierarchy(
        const UIHierarchyNodeUpdateParam& in_param,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
        const bool in_dirty,
        const bool in_render_to_texture,
        const bool in_always_dirty,
        const bool in_allow_clear,
        const VectorFloat4& in_clear_colour
        );

    void UpdateLayout(
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_target_size,
        const VectorInt2& in_target_offset,
        const VectorInt2& in_target_window
        );

    void UpdateResources(
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space
        );

    void MarkTextureDirty();

    const VectorInt2 GetTextureSize(
        DrawSystem* const in_draw_system
        ) const;

    void UpdateTextureSize(
        const VectorInt2& in_parent_size
        );

    //void UpdateSize(
    //    DrawSystem* const in_draw_system,
    //    const VectorInt2& in_parent_size,
    //    const VectorInt2& in_parent_offset,
    //    const VectorInt2& in_parent_window,
    //    const float in_ui_scale,
    //    const float in_time_delta,
    //    const bool in_mark_dirty,
    //    const UIScreenSpace& in_parent_screen_space
    //    );

    const bool DealInput(
        UIRootInputState& in_input_state,
        const bool in_parent_inside,
        const bool in_action = false
        );


    /// split PreDraw and Draw to allow UIComponentEffect to prep the childrent but implement it's own texture draw
    /// return True if we needed to draw, ie, we have modified children. Ask children to update there textures
    const bool PreDraw(
        const UIManagerDrawParam& in_draw_param
        );
    /// return True if we needed to draw, ie, we have modified _texture. Draw children textures to our own texture
    const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        const bool in_dirty
        );

    UITexture& GetUITexture() const { return *_texture; }

private:
    /// recursion data structure, holds a UIHierarchyNode, component, geometry...
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;

    /// array of effects on the node
    std::vector<std::shared_ptr<UIEffectComponent>> _array_effect_component;

    /// Hold the render target or wrap the backbuffer as a texture
    std::unique_ptr<UITexture> _texture;

    /// Allow top level node to hold some state for input like the focused node, or click active node, hover node?
    std::unique_ptr<UIRootInputState> _root_input_state;

};
