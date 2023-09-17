#pragma once
class DrawSystem;
class IRenderTarget;
class IResource;
class Shader;
class GeometryGeneric;

class DrawSystemFrame
{
public:
    DrawSystemFrame(DrawSystem&in_draw_system);
    ~DrawSystemFrame();
    ID3D12GraphicsCommandList* GetCommandList();
    // Was trying to hide device, but resizing render texture?
    //ID3D12Device2* const GetDevice();
    const int GetBackBufferIndex();
    void SetRenderTarget(IRenderTarget* const in_render_target, const bool in_allow_clear = true);
    void SetShader(Shader* const in_shader);
    void Draw(GeometryGeneric* const in_geometry);
    // thread_group_count as the number of steps for each thread to do
    void Dispatch(
        uint32_t in_thread_group_count_x,
        uint32_t in_thread_group_count_y = 1,
        uint32_t in_thread_group_count_z = 1
        );

    void ResourceBarrier(
        IResource* const in_resource, 
        D3D12_RESOURCE_STATES in_after_state
        );

    void UpdateGeometryGeneric(
        GeometryGeneric* const in_geometry,
        const std::vector<uint8_t>& in_vertex_data_raw
        );

private:
    DrawSystem&_draw_system;
    ID3D12GraphicsCommandList* _command_list;
    ID3D12CommandQueue* _command_queue;
    IRenderTarget* _render_target;
};
