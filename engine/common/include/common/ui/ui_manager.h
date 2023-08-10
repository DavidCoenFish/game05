#pragma once

#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class IRenderTarget;
class UIHierarchyNode;
class UIManagerImplementation;
class VectorInt2;
class VectorInt4;

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

    static std::shared_ptr<GeometryGeneric> GeometryHelper(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
        );
    static void GeometryUpdateHelper(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        GeometryGeneric* const in_geometry,
        // Left bottom, right top (pos, uv)
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f) // atention Y inverted
        );

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
        const float in_ui_scale, //in_pixels_per_em//pixels per em?,
        const bool in_needs_to_draw = true// If something else is drawing to the render target, we always need to draw
        );

private:
    std::unique_ptr<UIManagerImplementation> _implementation;

};
