#pragma once
///

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class IUIComponent;
class UIData;
class IUIModel;
class LocaleSystem;
class Shader;
class TextManager;
class UIHierarchyNode;
class UIManager;
class UIManagerImplementation;

//enum class LocaleISO_639_1;
enum class UIEffectEnum;
enum class UIShaderEnum;

struct UIDataTextRunStyle;

/// \related UIManager
struct UIComponentFactoryParam
{
    explicit UIComponentFactoryParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        TextManager* const in_text_manager = nullptr,
        const int in_create_index = 0
        );
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    TextManager* _text_manager;
    int _create_index;
};

///  locale should be an aspect of the UIData, not the ui update param
///  however, Harfbuz want to know the locale to help pos the text.
///  so Locale in is used to resolve the locale system keys to text, but we still pass locale key
/// \related UIManager
struct UIManagerUpdateParam
{
    explicit UIManagerUpdateParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const IUIModel* const in_ui_model = nullptr,
        const UIDataTextRunStyle* const in_default_text_style = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const float in_ui_scale = 1.0f,
        const float in_time_delta = 0.0f,
        const bool in_draw_every_frame = false, // mark top level as dirty each frame, the destination render target may have other systems drawing to it
        const bool in_draw_to_texture = false, // Draw to texture or backbuffer?
        const VectorInt2& in_texture_size = VectorInt2(0,0), // If in_draw_to_texture is true, size to use for texture
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );

    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    const UIDataTextRunStyle* const _default_text_style;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    float _ui_scale;
    float _time_delta;
    bool _draw_to_texture;
    bool _draw_every_frame;
    VectorInt2 _texture_size;
    bool _allow_clear;
    VectorFloat4 _clear_colour;

};

/// \related UIManager
/// Does UIRootInputState hold this data?
struct UIManagerDealInputParam
{
    static UIManagerDealInputParam Factory();

    explicit UIManagerDealInputParam(
        DrawSystem* const in_draw_system = nullptr,
        const bool in_mouse_valid = false,
        const bool in_mouse_left_down = false,
        const bool in_mouse_right_down = false,
        const int in_mouse_x = 0,
        const int in_mouse_y = 0,
        const int in_mouse_scroll = 0
        );

    //int _navigation_left_repeat;
    //int _navigation_right_repeat;
    //int _navigation_up_repeat;
    //int _navigation_down_repeat;
    //bool _action_currently_down;
    //bool _action_transition_up; //action key released this update

    DrawSystem* _draw_system;
    bool _mouse_valid;
    bool _mouse_left_down;
    bool _mouse_right_down;
    int _mouse_x;
    //int _mouse_x_delta;
    int _mouse_y;
    //int _mouse_y_delta;
    int _mouse_scroll;

};

/// \related UIManager
struct UIManagerDrawParam
{
    explicit UIManagerDrawParam(
        DrawSystem* const in_draw_system = nullptr,
        DrawSystemFrame* const in_frame = nullptr,
        TextManager* const in_text_manager = nullptr,
        UIManager* const in_ui_manager = nullptr//,
        //const float in_ui_scale = 1.0f
        );

    DrawSystem* const _draw_system;
    DrawSystemFrame* const _frame;
    TextManager* const _text_manager;
    UIManager* const _ui_manager;
   // const float _ui_scale;

    // Need some way of detecticting if device was reset
    //const bool in_force_total_redraw = false
};

/// \class UIManager
/// \brief A minimal redraw ui system, heirarchy based
class UIManager
{
public:
    struct TShaderConstantBuffer
    {
        VectorFloat4 _tint_colour;
        //float _tint_colour[4];
    };

    UIManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~UIManager();

    // UIGeometry is private, so expose via UIManager
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescArray();
    static void BuildGeometryData(
        std::vector<uint8_t>& out_vertex_data,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f) // atention Y inverted
        );

    // Add content factories, layout templates can be expressed through the content class
    typedef std::function< const bool(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_param
        )> TContentFactory;
    // Clear flag and background colour moved to IUIComponent
    void AddContentFactory(
        const std::string& in_content_name,
        const TContentFactory& in_factory
        );

    /// Update [heirearchy, desired size, layout], in_out_target can start out as null to kick things off
    /// Don't recurse through this method, intended for kicking off the root node
    void Update(
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        const UIManagerUpdateParam& in_param,
        const std::string& in_model_key = ""
        );

    /// root node owns the input state
    void DealInput(
        UIHierarchyNode& in_root,
        const UIManagerDealInputParam& in_param
        );

    /// root node references the top level texture or backbuffer
    void Draw(
        UIHierarchyNode& in_root,
        const UIManagerDrawParam& in_param
        );

    const std::shared_ptr<Shader>& GetShaderRef(const UIShaderEnum in_type) const;

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};
