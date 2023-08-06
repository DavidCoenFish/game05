#pragma once

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class IRenderTarget;
class UIHierarchyNode;
class UIManagerImplementation;
class VectorInt2;
class VectorInt4;
class VectorFloat4;

class UIManager
{
public:
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc();

    UIManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~UIManager();

    // Expose block drawing to current frame and whatever render target is already set on the frame
    void DrawHelper(
        DrawSystemFrame* const in_frame,
        IRenderTarget* const in_render_target,
        GeometryGeneric* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_texture_handle
        );

    void DrawHierarchy(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        UIHierarchyNode* const in_node,
        const float in_ui_scale
        );

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};
