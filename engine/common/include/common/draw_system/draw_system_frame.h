#pragma once
class DrawSystem;
class DrawSystemResourceList;
class GeometryGeneric;
class IRenderTarget;
class IResource;
class RenderTargetTexture;
class Shader;
class ShaderConstantBuffer;

class DrawSystemFrame
{
public:
	DrawSystemFrame(DrawSystem& in_draw_system);
	~DrawSystemFrame();
	ID3D12GraphicsCommandList* GetCommandList();

	const int GetBackBufferIndex();

	/// Warning, IRenderTarget is not an IResource, if we are a RenderTargetTexture, call AddFrameResource upstream
	/// needed in_allow_clear as a render target which is set to clear on being set, may be set more than once
	void SetRenderTarget(
		IRenderTarget* const in_render_target,
		const bool in_allow_clear = true
		);

	void SetRenderTargetTexture(
		const std::shared_ptr<RenderTargetTexture>& in_render_target,
		const bool in_allow_clear = true
		);

	/// use case: a texture used by a shader
	void AddFrameResource(
		const std::shared_ptr<IResource>& in_resource
		);

	/// pass ref smart pointer as need to extend resource lifespan till command list completed
	void SetShader(
		const std::shared_ptr<Shader>& in_shader,
		const std::shared_ptr<ShaderConstantBuffer>& in_shader_constant_buffer = nullptr
		);

	/// pass ref smart pointer as need to extend resource lifespan till command list completed
	void Draw(
		const std::shared_ptr<GeometryGeneric>& in_geometry
		);

	/// Trigger compute shader, thread_group_count as the number of steps for each thread to do
	void Dispatch(
		uint32_t in_thread_group_count_x,
		uint32_t in_thread_group_count_y = 1,
		uint32_t in_thread_group_count_z = 1
		);

	/// Transition a given resource
	void ResourceBarrier(
		IResource* const in_resource, 
		D3D12_RESOURCE_STATES in_after_state
		);

	void UpdateGeometryGeneric(
		const std::shared_ptr<GeometryGeneric>& in_shader,
		const std::vector<uint8_t>& in_vertex_data_raw
		);

private:
	/// draw system reference
	DrawSystem& _draw_system;

	/// reference to the command list of this frame
	ID3D12GraphicsCommandList* _command_list;

	/// keep reference of current render target
	IRenderTarget* _render_target;

	/// The list of resources we need to keep alive till command list finishes on GPU
	std::shared_ptr<DrawSystemResourceList> _resource_list;
};
