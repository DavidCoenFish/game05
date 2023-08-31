#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class IUIContent;
class IUIData;
class IUIModel;
class LocaleSystem;
class TextManager;
class UIHierarchyNode;
class UIManager;
class UIManagerImplementation;

struct UIContentFactoryParam
{
    explicit UIContentFactoryParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        TextManager* const in_text_manager = nullptr
        );
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    TextManager* _text_manager;
};

struct UIManagerUpdateParam
{
    explicit UIManagerUpdateParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const IUIModel* const in_ui_model = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const float in_ui_scale = 1.0f,
        const float in_time_delta = 0.0f,
        const std::string& in_locale = std::string(),
        const bool in_draw_to_texture = false, // Draw to texture or backbuffer?
        const bool in_always_dirty = false,
        const VectorInt2& in_texture_size = VectorInt2(0,0) // If in_draw_to_texture is true, size to use for texture
        );

    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    float _ui_scale;
    float _time_delta;
    std::string _locale;
    bool _draw_to_texture;
    bool _always_dirty;
    VectorInt2 _texture_size;

};

// Does UIRootInputState hold this data?
struct UIManagerDealInputParam
{
    int _navigation_left_repeat;
    int _navigation_right_repeat;
    int _navigation_up_repeat;
    int _navigation_down_repeat;
    bool _action_currently_down;
    bool _action_transition_up; //action key released this update

    bool _mouse_valid;
    bool _mouse_left_down;
    bool _mouse_right_down;
    int _mouse_x;
    int _mouse_x_delta;
    int _mouse_y;
    int _mouse_y_delta;
    float _mouse_scroll;

};

struct UIManagerDrawParam
{
    explicit UIManagerDrawParam(
        DrawSystem* const in_draw_system = nullptr,
        DrawSystemFrame* const in_frame = nullptr,
        TextManager* const in_text_manager = nullptr
        );

    DrawSystem* const _draw_system;
    DrawSystemFrame* const _frame;
    TextManager* const _text_manager;

    // Need some way of detecticting if device was reset
    //const bool in_force_total_redraw = false
};

class UIManager
{
public:
    UIManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~UIManager();

    // Expose the input element desc for the ui manager shader to allow more use of the shader
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescArray();
    static void BuildGeometryData(
        std::vector<uint8_t>& out_vertex_data,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f) // atention Y inverted
        );

    //Add content factories, layout templates can be expressed through the content class
    typedef std::function< const bool(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )> TContentFactory;
    // Clear flag and background colour moved to IUIContent
    void AddContentFactory(
        const std::string& in_content_name,
        const TContentFactory& in_factory
        );

    // Update [heirearchy, desired size, layout], in_out_target can start out as null to kick things off
    // Don't recurse through this method, intended for kicking off the root node
    void Update(
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        const UIManagerUpdateParam& in_param,
        const std::string& in_model_key = ""
        );

    // Deal input
    void DealInput(
        UIHierarchyNode& in_root,
        const UIManagerDealInputParam& in_param
        );

    // Draw
    void Draw(
        UIHierarchyNode& in_root,
        const UIManagerDrawParam& in_param
        );

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};
