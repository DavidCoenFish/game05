#pragma once

class DrawSystem;
class DrawSystemFrame;
class UiBlockImplementation;
class VectorInt2;
class VectorFloat4;
class HeapWrapperItem;

class UiBlock
{
public:
    UiBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size,
        const bool in_clear_on_set,
        const VectorFloat4& in_clear_color
        );
    ~UiBlock();
    std::shared_ptr<HeapWrapperItem> GetTexture(
        DrawSystemFrame* const in_frame
        );

    void Activate(
        DrawSystemFrame* const in_frame
        );

    // Resize()
    // SetActive(DrawSystemFrame)

private:
    std::unique_ptr<UiBlockImplementation> _implementation;

};
