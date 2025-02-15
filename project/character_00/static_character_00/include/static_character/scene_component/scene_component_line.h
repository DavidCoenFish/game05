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

class SceneComponentLine
{
public:
    static std::shared_ptr<SceneComponentLine> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path,
        const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
		const VectorFloat3& in_point_a,
		const VectorFloat3& in_point_b,
		const VectorFloat4& in_colour,
		const float in_line_thickness = 1.0f
        );
    SceneComponentLine(
        const std::shared_ptr<Shader>& in_shader_sphere,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_line_constant_buffer
        );
    ~SceneComponentLine();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

	void SetLine(
		const VectorFloat3& in_point_a,
		const VectorFloat3& in_point_b
		);

private:
    std::shared_ptr<Shader> _shader_line;
    std::shared_ptr<ShaderConstantBuffer> _shader_line_constant_buffer;

};
