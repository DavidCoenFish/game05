#pragma once

#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class IUIModel;
class UIManager;
class UIManagerImplementation;

struct UIManagerTemplateFactoryParam
{
    DrawSystem* _draw_system;
    ID3D12GraphicsCommandList* _command_list;
};

struct UIManagerUpdateLayoutParam
{
    DrawSystem* _draw_system;
    ID3D12GraphicsCommandList* _command_list;
    IUIModel* _ui_model;
    UIManager* _ui_manager;
    float _time_delta;
    float _ui_scale;
    std::string _locale;
};

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
    DrawSystem* const _draw_system;
    DrawSystemFrame* _frame;

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

    // Get the InputElementDescArray to match the ui manager shader
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescArray();
    static void BuildGeometryData(
        std::vector<uint8_t>& out_vertex_data,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f) // atention Y inverted
        );

    //Add template
    typedef std::function< void(
        std::shared_ptr<UIHierarchyNode>& in_out_target,
        UIManagerTemplateFactoryParam& in_param
        )> TTemplateFactory;
    void AddTemplateFactory(
        const std::string& in_template_name,
        const TTemplateFactory& in_factory
        );

    // Update layout
    void UpdateLayout(
        std::shared_ptr<UIHierarchyNode>& in_out_target,
        UIManagerUpdateLayoutParam& in_param,
        const std::string& in_model_key,
        // Should the top level node draw to the backbuffer, or an internal texture
        const bool in_draw_to_texture = false
        );

    // Deal input
    void DealInput(
        UIHierarchyNode& in_root,
        UIManagerDealInputParam& in_param
        );

    // Draw
    void Draw(
        UIHierarchyNode& in_root,
        UIManagerDrawParam& in_param
        );

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};








#if 0

#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class IRenderTarget;
class IUIProviderData;
class UIHierarchyNode;
class UIManager;
class UIManagerImplementation;
class VectorInt2;
class VectorInt4;
struct UIHierarchyNodeChildData;

struct UIManagerDrawData
{
    explicit UIManagerDrawData(
        const float in_ui_scale = 8.0f,
        const std::string& in_locale = "",
        const float in_time_delta = 0.0f,
        const IUIProviderData* const in_ui_provider_data = nullptr,
        UIManager* const in_ui_manager = nullptr
        );
    float _ui_scale;
    std::string _locale;
    float _time_delta;
    const IUIProviderData* const _ui_provider_data;
    UIManager* const _ui_manager;
    //i_ui_data_server
    // access locale data via data server, ie, data server gives us string, it can deal with locale
};

class UIManager
{
public:
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc();

    UIManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~UIManager();

    static std::shared_ptr<GeometryGeneric> GeometryHelper(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
        );
    static void GeometryUpdateHelper(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        GeometryGeneric* const in_geometry,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f) // atention Y inverted
        );

    // Expose block drawing to current frame and whatever render target is already set on the frame
    void DrawHelper(
        DrawSystemFrame* const in_frame,
        IRenderTarget* const in_render_target,
        GeometryGeneric* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_texture_handle
        );

    void DrawHierarchy(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        UIHierarchyNode* const in_node,
        const UIManagerDrawData& in_data
        );
#if 0
    void AddHierarchyNodeFactory(
        const std::string& in_template_name,
        const std::function< std::shared_ptr<UIHierarchyNode>(
            UIManager* const,
            DrawSystem* const,
            ID3D12GraphicsCommandList* const,
            IUIProviderData* const,
            const std::string& // key_base
            )>& in_factory
        );

    std::shared_ptr<UIHierarchyNode> MakeHierarchyNode(
        const std::string& in_template_name,
        const std::string& in_provider_data_key_base,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        IUIProviderData* const in_provider_data
        );

    void BuildPage(
        UIHierarchyNode* const in_parent,
        const std::string& in_template_name, 
        const std::string& in_provider_data_key_base,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        IUIProviderData* const in_provider_data
        );
#else
    typedef std::function<
        void(
            UIManager* const in_ui_manager,
            UIHierarchyNode& in_parent_node, // call AddChild, ClearChildren
            const IUIData* const in_ui_data,
            const IUIProviderData* const in_provider_data,
            const std::string& in_provider_data_key_base,
            DrawSystem* const in_draw_system,
            ID3D12GraphicsCommandList* const in_command_list,
        )> TNodeFactory;

    void AddHierarchyNodeFactory(
        const std::string& in_template_name,
        const TNodeFactory& in_node_factory
        );

    void AppendHierarchyNodeChild(
        UIHierarchyNode& in_parent_node, // call AddChild, ClearChildren
        const IUIData* const in_ui_data, // get template name to map to NodeFactory
        const IUIProviderData* const in_provider_data,
        const std::string& in_provider_data_key_base,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );

#endif
private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};

#endif