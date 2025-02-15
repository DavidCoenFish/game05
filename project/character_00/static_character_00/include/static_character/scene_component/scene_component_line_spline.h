#pragma once

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class ShaderConstantBuffer;
class VectorFloat3;
class VectorFloat4;

struct CameraConstantBufferB0;

class SceneComponentLineSpline
{
public:
    SceneComponentLineSpline(
        const std::shared_ptr<Shader>& in_shader,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_constant_buffer
        );
    ~SceneComponentLineSpline();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

private:
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};
