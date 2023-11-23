#pragma once
///

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_enum.h"

class DrawSystem;
class DrawSystemFrame;
class IUIComponent;
class IUIModel;
class LocaleSystem;
class Shader;
class TextManager;
class UIComponentTooltipLayer;
class UIData;
class UIGeometry;
class UIHierarchyNode;
class UIManager;
class UIManagerImplementation;

//enum class LocaleISO_639_1;
enum class UIEffectEnum;
enum class UIShaderEnum;
enum class UITouchFlavour;
enum class UINavigationType;

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
///  so Locale is used to resolve the locale system keys to text on the UIData side, but we still pass locale key for harfbuz
/// \related UIManager
struct UIManagerUpdateParam
{
    explicit UIManagerUpdateParam(
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const UIDataTextRunStyle* const in_default_text_style = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const float in_ui_scale = 1.0f,
        const float in_time_delta = 0.0f,
        const bool in_draw_every_frame = false, // mark top level as dirty each frame, the destination render target may have other systems drawing to it
        const bool in_draw_to_texture = false, // Draw to texture or backbuffer?
        const VectorInt2& in_texture_size = VectorInt2(0,0) // If in_draw_to_texture is true, size to use for texture
        );

    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const UIDataTextRunStyle* const _default_text_style;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    float _ui_scale;
    float _time_delta;
    bool _draw_to_texture;
    bool _draw_every_frame;
    VectorInt2 _texture_size;

};

/// \related UIManager
/// Treat mouse right as a seperate touch?
/// Touches can have flavours, shift,control,alt key down? left or right mouse?
struct UIManagerDealInputTouch
{
    explicit UIManagerDealInputTouch(
        const VectorInt2& in_root_relative_pos = VectorInt2(),
        const bool in_active = false,
        const int in_id = 0,
        const UITouchFlavour in_flavour = UITouchFlavour::TNone
        );

    /// Pixel coords relative to application window
    VectorInt2 _root_relative_pos;
    /// Mouse button down/ finger touching screen
    bool _active; 
    /// Each continous touch gets an id, may be zero for mouse move and mouse left button
    int _id;
    /// Hint on touch type
    UITouchFlavour _flavour;
};

/// \related UIManager
struct UIManagerDealInputNavigation
{
    explicit UIManagerDealInputNavigation(
        const UINavigationType in_type = UINavigationType::TNone,
        const float in_amount = 1.0f,
        const bool in_end = false
        );

    /// Key type
    UINavigationType _type;
    /// Duration of key down
    //float _duration; //duration gets calculated internally?
    /// joystick or scroll amount, else 1.0 for keypress?
    float _amount; 
    /// True if the key down ended this frame
    bool _end;
};

/// \related UIManager
/// Does UIRootInputState hold this data?
struct UIManagerDealInputParam
{
    explicit UIManagerDealInputParam(
        const std::vector<UIManagerDealInputTouch>& in_touch_array = std::vector<UIManagerDealInputTouch>(),
        const std::vector<UIManagerDealInputNavigation>& in_navigation_array = std::vector<UIManagerDealInputNavigation>(),
        DrawSystem* const in_draw_system = nullptr,
        const float in_time_delta = 0.0f
        );

    std::vector<UIManagerDealInputTouch> _touch_array;
    std::vector<UIManagerDealInputNavigation> _navigation_array;
    DrawSystem* _draw_system;
    float _time_delta;

};

/// \related UIManager
struct UIManagerDrawParam
{
    explicit UIManagerDrawParam(
        DrawSystem* const in_draw_system = nullptr,
        DrawSystemFrame* const in_frame = nullptr,
        TextManager* const in_text_manager = nullptr,
        UIManager* const in_ui_manager = nullptr
        );

    DrawSystem* const _draw_system;
    DrawSystemFrame* const _frame;
    TextManager* const _text_manager;
    UIManager* const _ui_manager;

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

    /// Update [heirearchy, layout], in_out_target can start out as null to kick things off
    /// Don't recurse through this method, intended for kicking off the root node
    /// Initially wanted to provide an array of UIData to populate the children of the target, 
    /// But want a top level UIData to be the endpoint for the [hierarchy, layout, render] flags
    void Update(
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        //UIData* const in_root_ui_data,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
        const UIManagerUpdateParam& in_param,

        const bool in_render_to_texture, // if false, rather than render to texture, we render to the draw system backbuffer
        const VectorInt2& in_render_texture_size, // if in_render_to_texture is true, 
        const bool in_always_dirty = false, //still draw to the top level target even if nothing has changed
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );

    /// root node owns the input state
    void DealInput(
        UIHierarchyNode& in_root,
        UIData* const in_tooltip_layer,
        UIData* const in_combo_box_layer,
        const UIManagerDealInputParam& in_param
        );

    /// root node references the top level texture or backbuffer
    void Draw(
        UIHierarchyNode& in_root,
        const UIManagerDrawParam& in_param
        );

    const std::shared_ptr<Shader>& GetShaderRef(const UIShaderEnum in_type) const;
    const UIGeometry& GetEffectGeometryRef() const;

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};
