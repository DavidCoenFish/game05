#pragma once
class DrawSystem;
class IRenderTarget;
class Shader;
class GeometryGeneric;

class DrawSystemFrame
{
public:
    DrawSystemFrame(DrawSystem&in_draw_system);
    ~DrawSystemFrame();
    ID3D12GraphicsCommandList* GetCommandList();
    const int GetBackBufferIndex();
    void SetRenderTarget(IRenderTarget* const in_render_target);
    void SetShader(Shader* const in_shader);
    void Draw(GeometryGeneric* const in_geometry);
    void Dispatch(
        uint32_t in_num_groups_x,
        uint32_t in_num_groups_y = 1,
        uint32_t in_num_groups_z = 1
        );

    void ResourceBarrier(
        IResource* const in_resource, 
        D3D12_RESOURCE_STATES in_after_state
        );

private:
    DrawSystem&_draw_system;
    ID3D12GraphicsCommandList* _command_list;
    ID3D12CommandQueue* _command_queue;
    IRenderTarget* _render_target;
};
