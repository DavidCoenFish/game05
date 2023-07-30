#pragma once

class UiBlockImplementation;

class DrawSystem;
class VectorInt2;
class HeapWrapperItem;

class UiBlock
{
public:
    UiBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size
        );
    ~UiBlock();
    std::shared_ptr<HeapWrapperItem> GetTexture() const;

    // Resize()
    // SetActive(DrawSystemFrame)

private:
    std::unique_ptr<UiBlockImplementation> _implementation;

};
