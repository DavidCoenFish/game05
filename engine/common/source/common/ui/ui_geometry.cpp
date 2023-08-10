#include "common/common_pch.h"
#include "common/ui/ui_geometry.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/ui/ui_manager.h"
#include "common/util/vector_helper.h"

namespace
{
    void GeneratedVertexData(
        std::vector<uint8_t>& out_vertex_raw_data,
        const VectorFloat4 in_pos,
        const VectorFloat4 in_uv
        )
    {
        //0.0f, 0.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[1]);

        //0.0f, 1.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[1]);

        //1.0f, 0.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[3]);

        //1.0f, 1.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv[3]);

        return;
    }
}

std::shared_ptr<GeometryGeneric> UIGeometry::GeometryHelper(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    std::vector<uint8_t> vertex_data;
    GeneratedVertexData(vertex_data, in_pos, in_uv);

    auto geometry = in_draw_system->MakeGeometryGeneric(
        in_command_list,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        UIManager::GetInputElementDesc(),
        vertex_data,
        4
    );
    return geometry;
}

void UIGeometry::GeometryUpdateHelper(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    GeometryGeneric* const in_geometry,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    std::vector<uint8_t> vertex_data;
    GeneratedVertexData(vertex_data, in_pos, in_uv);

    in_draw_system->UpdateGeometryGeneric(
        in_command_list,
        in_geometry,
        vertex_data
        );
}

UIGeometry::UIGeometry(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
    : _pos(in_pos)
    , _uv(in_uv)
{
    _geometry = GeometryHelper(
        in_draw_system,
        in_command_list,
        in_pos,
        in_uv
        );
}

UIGeometry::~UIGeometry()
{
    // Nop
}

const bool UIGeometry::Update(
    DrawSystemFrame* const in_frame,
    const VectorFloat4 in_pos,
    const VectorFloat4 in_uv
    )
{
    if ((_uv == in_uv) &&
        (_pos == in_pos))
    {
        return false;
    }

    std::vector<uint8_t> vertex_data;
    GeneratedVertexData(vertex_data, _pos, _uv);

    in_frame->UpdateGeometryGeneric(
        _geometry.get(),
        vertex_data
        );
    return true;
}

GeometryGeneric* UIGeometry::GetGeometry()
{
    return _geometry.get();
}

