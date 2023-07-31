#pragma once

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UiManagerImplementation;
class VectorInt2;
class VectorInt4;
class VectorFloat4;

class UiManager
{
public:
    UiManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~UiManager();

    // Expose block drawing to current frame and whatever render target is already set on the frame
    void DrawBlock(
        DrawSystemFrame* const in_frame,
        const VectorInt2& in_target_size,
        const VectorInt4& in_block_size,
        const VectorFloat4& in_block_uv,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );

private:
    std::unique_ptr<UiManagerImplementation> _implementation;

};
