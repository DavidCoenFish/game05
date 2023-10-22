#pragma once
#include "common/math/vector_float3.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class ShaderConstantBuffer;
class ShaderResource;
class UnorderedAccess;

struct CameraConstantBufferB0;

class SceneComponentBackground
{
public:
    static std::shared_ptr<SceneComponentBackground> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path,
        const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture
        );
    SceneComponentBackground(
        const std::shared_ptr<Shader>& in_shader_background,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_background_constant_buffer,
        const std::shared_ptr<Shader>& in_shader_grid,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_grid_constant_buffer
        );
    ~SceneComponentBackground();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

private:
    std::shared_ptr<Shader> _shader_background;
    std::shared_ptr<ShaderConstantBuffer> _shader_background_constant_buffer;
    
    std::shared_ptr<Shader> _shader_grid;
    std::shared_ptr<ShaderConstantBuffer> _shader_grid_constant_buffer;

};
