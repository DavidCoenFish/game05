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

class SceneComponentSphere
{
public:
    static std::shared_ptr<SceneComponentSphere> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path,
        const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
		const VectorFloat4& in_colour,
		const VectorFloat4& in_sphere_xyz_radius
        );
    SceneComponentSphere(
        const std::shared_ptr<Shader>& in_shader_sphere,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_sphere_constant_buffer
        );
    ~SceneComponentSphere();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

	void SetSphere(
		const VectorFloat4& in_sphere_xyz_radius
		);

private:
    std::shared_ptr<Shader> _shader_sphere;
    std::shared_ptr<ShaderConstantBuffer> _shader_sphere_constant_buffer;

};
