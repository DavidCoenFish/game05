#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class GeometryGeneric;
class Shader;
class ShaderResource;
class SceneComponentScreenQuad;

class ApplicationTestText : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestText(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestText();

private:
    virtual void Update() override;

    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResources
    {
        std::shared_ptr<SceneComponentScreenQuad> _screen_quad;
        std::shared_ptr<Shader> _background_shader;

        std::shared_ptr<Shader> _text_shader;
        std::shared_ptr<ShaderResource> _text_shader_resource;
        std::shared_ptr<GeometryGeneric> _text_geometry;
    };
    std::unique_ptr<DrawResources> _draw_resources;

    struct Cell
    {
        int _channel;
        int _rect[4];
        // int _ref_count;
    };

    struct Row
    {
        int _channel;
        int _start_y;
        int _height;
        std::vector<Cell> _cell_data;
        int _last_x;
    };

    // For a given font and size, have a map of glyph id to texture data
    std::map<uint32_t, Cell*> _glyph_cell_map;
    std::vector<std::shared_ptr<Row>> _row_data;

    std::vector<uint8_t> _glyph_pixel_data;
    bool _glyph_pixel_data_dirty;
    int _glyph_pixel_data_dirty_range_y_low;
    int _glyph_pixel_data_dirty_range_y_high;

    std::string _text;
    bool _text_dirty;

    int _screen_width;
    int _screen_height;

};
