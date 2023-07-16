#pragma once
#include "common/window/i_window_application.h"
#include "common/draw_system/geometry/geometry.h"

class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTriangle : public IWindowApplication
{
public:
    struct Vertex
    {
    public:
        DirectX::XMFLOAT2 _position;
        DirectX::XMFLOAT4 _color;
    };
    ApplicationTriangle(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTriangle();
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

public:
    std::shared_ptr < Geometry < Vertex > > _geometry;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr < DrawSystem > _draw_system;
    std::shared_ptr < Shader > _shader;
};
