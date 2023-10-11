#pragma once
#include "common/math/vector_int2.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class ShaderResource;
class RenderTargetTexture;
class GeometryGeneric;
class UIManager;
class UIHierarchyNode;
class UIComponentTexture;

class ApplicationTestCoord : public IWindowApplication
{
public:
    ApplicationTestCoord(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestCoord();
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResource
    {
        std::unique_ptr<UIManager> _ui_manager;

        std::shared_ptr<Shader> _triangle_shader;
        std::shared_ptr<GeometryGeneric> _triangle_geometry;

        std::shared_ptr<RenderTargetTexture> _render_target;
        std::shared_ptr<ShaderResource> _texture;

        std::shared_ptr<Shader> _present_shader;
        std::shared_ptr<GeometryGeneric> _panel_geometry[2];

        std::shared_ptr<UIHierarchyNode> _ui_hierarchy_node;
        std::shared_ptr<UIComponentTexture> _ui_texture[2];
        
    };
    std::unique_ptr<DrawResource> _draw_resource;

    VectorInt2 _screen_size;

};
