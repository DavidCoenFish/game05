#pragma once

#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;

class UIGeometry
{
public:
    static std::shared_ptr<GeometryGeneric> GeometryHelper(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
        );

    UIGeometry(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list, 
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
        );
    ~UIGeometry();

    // Return true if geometry changed/ updated
    const bool Update(
        DrawSystemFrame* const in_frame,
        const VectorFloat4 in_pos,
        const VectorFloat4 in_uv
        );

    GeometryGeneric* GetGeometry();

private:
    std::shared_ptr<GeometryGeneric> _geometry;
    //bool _data_changed;
    VectorFloat4 _pos;
    VectorFloat4 _uv;

};
