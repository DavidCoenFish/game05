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

class SceneComponentGrid
{
public:
    static std::shared_ptr<SceneComponentGrid> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path,
        const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture
        );
    SceneComponentGrid(
        const std::shared_ptr<Shader>& in_shader_grid,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_grid_constant_buffer
        );
    ~SceneComponentGrid();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

private:
    std::shared_ptr<Shader> _shader_grid;
    std::shared_ptr<ShaderConstantBuffer> _shader_grid_constant_buffer;

};
