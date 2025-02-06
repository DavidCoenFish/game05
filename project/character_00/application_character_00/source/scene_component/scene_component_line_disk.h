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

class SceneComponentLineDisk
{
public:
    static std::shared_ptr<SceneComponentLineDisk> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path,
        const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
		const VectorFloat3& in_pos,
		const float in_radius,
		const VectorFloat3& in_normal,
		const VectorFloat4& in_colour,
		const float in_thickness = 1.0f
        );
    SceneComponentLineDisk(
        const std::shared_ptr<Shader>& in_shader_sphere,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_line_constant_buffer
        );
    ~SceneComponentLineDisk();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

	void SetData(
		const VectorFloat3& in_pos,
		const float in_radius,
		const VectorFloat3& in_normal,
		const float in_thickness = 1.0f
		);

private:
    std::shared_ptr<Shader> _shader_line;
    std::shared_ptr<ShaderConstantBuffer> _shader_line_constant_buffer;

};
