#pragma once

#include "common/ui/ui_coord.h"
#include "common/ui/ui_enum.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UIData;
class UIEffectData;
class UIGeometry;
class UIHierarchyNode;
class UIHierarchyNodeChild;
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

struct UIHierarchyNodeUpdateParam
{
    explicit UIHierarchyNodeUpdateParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        UIManager* const in_ui_manager = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const UIDataTextRunStyle* const in_default_text_style = nullptr,
        const float in_ui_scale = 1.0f,
        const float in_delta_time_seconds = 0.0f
        );
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    UIManager* const _ui_manager;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    const UIDataTextRunStyle* const _default_text_style;
    float _ui_scale;
    float _delta_time_seconds;

};

/// to allow a node to be used as the top level document root, which doen't need geometry for how it draws to a parent as it is the top level
/// UIHierarchyNode has been cut into UIHierarchyNode and UIHierarchyNodeChild. 
/// the UIHierarchyNodeChild holds a UIHierarchyNode node, and a component, and some other data
/// UIHierarchyNode holds an array of child UIHierarchyNodeChild, and the texture, effects on the texture, (and an optional root input state)
///
///   v2 
/// UIHierarchyNode // N0 
///     <_input state>
///     _texture // T0 the texture for N0, the children A0 draw to this using there geometry G1. size is from external, such as screen size
///     _effect_stack // E0, use T0 as input, and perform an effect step for each item in stack
///     // if we had layout here, it would be redundant for root node
///     _child_node_array // A0, UIHierarchyNodeChild
///     [
///         _component // C0, special rules for layout and rendering for each different component type
///                    // tooltip manager may want to intercept update hierarchy, else it could modify the children of the uidata is it matched with? callback, or store changes to update called?
///         _layout/other common stuff of _component to allow component to move to pure interface rather than have inherited functionality
///         _shader_constant_buffer
///         _geometry // G0
///         _node // N1, recursion point
///     ]
///
/// seem to be moving towards UIHierarchyNode implementing hierarchy step, 
/// and UIHierarchyNodeChild implmenting common functionality,
/// and IUIComponent implementing specialisation

class UIHierarchyNode
{
public:
    UIHierarchyNode();
    ~UIHierarchyNode();

    UIRootInputState& GetOrMakeRootInputState();

    /// nullptr if _bUseBackBuffer is true
    //std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    //void AddChild(
    //    std::unique_ptr<IUIComponent>& in_content
    //    );
    //const bool ClearChildren();

    /// Expose child data array to allow ui_component to specialise how hieararchy builds
    std::vector<std::shared_ptr<UIHierarchyNodeChild>>& GetChildData() { return _child_array; }

    /// create/ destroy nodes to match model, make component of in_parent_to_this_node_or_null match type from UIData in_data
    /// we do this dance around updateing the component of the parent child so that the root hierarchy node doesn't have a component, but still maps to a ui data
    /// previously we took in an array of ui data, but then also need the effect and texture param, which already had 
    void UpdateHierarchy(
        UIData& in_data,
        UIHierarchyNodeChild* const in_parent_to_this_node_or_null,
        const UIHierarchyNodeUpdateParam& in_param
        );

    /// UpdateLayout split from UpdateResources to allow texture size to be based on child size, rather than just top down layout size
    /// Use case, tooltips with a dropshadow margin around text
    void UpdateLayout(
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window,
        const VectorInt2& in_parent_offset
        );

    /// Return true if something changed and ui texture is now marked to draw, as need to propergate this up the hierarchy, 
    /// ie, need to mark parent nodes texture as dirty on child change
    /// Moved render dirty flag into IOHierarchyNodeChild
    void UpdateResources(
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space
        );

    void MarkTextureDirty();

    const VectorInt2 GetTextureSize(
        DrawSystem* const in_draw_system
        ) const;

    const bool SetTextureSize(
        const VectorInt2& in_texture_size
        );

    void DealInput(
        UIRootInputState& in_input_state,
        const bool in_parent_inside
        );
    /// split deal input to allow click to be on most nested not the top level
    void DealInputSetStateFlag(const UIStateFlag in_input_state_flag);

    /// Draw children textures to our own texture
    void Draw(
        const UIManagerDrawParam& in_draw_param,
        const UIStateFlag in_state_flag = UIStateFlag::TNone,
        const bool in_dirty = false
        );

    /// Get the texture for the node, could be the last effect of the effect stacks texture, or the nodes own ui texture
    UITexture& GetUITexture() const;// { return *_texture; }

    /// Get the base texture for the node, the texture before effect, ie, the texture for string compont to draw text to
    UITexture& GetBaseUITexture() const { return *_texture; }

private:
    /// currently this is under the Hierarchy update, so on effect stack change over in the UIData, it needs to set hierarchy dirty to get change propergated
    const bool ApplyEffect(
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
        const UIHierarchyNodeUpdateParam& in_param,
        bool& in_out_tint_array_in_use
        );

private:
    typedef std::vector<std::shared_ptr<UIHierarchyNodeChild>> TChildArray;
    /// recursion data structure, holds a UIHierarchyNode, component, geometry...
    TChildArray _child_array;

    /// array of effects on the node
    std::vector<std::shared_ptr<UIEffectComponent>> _array_effect_component;

    /// Hold the render target or wrap the backbuffer as a texture for this node's components to draw to
    std::unique_ptr<UITexture> _texture;

    /// Allow top level node to hold some state for input like the focused node, or click active node, hover node?
    /// this could be null for any node not used as a top level input host. 
    /// Alternative is to have the input state external to the node hierarchy, but was trying to have everything under the root node
    std::unique_ptr<UIRootInputState> _root_input_state;

};
