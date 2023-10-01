#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem&in_draw_system) 
    : _draw_system(in_draw_system)
    , _command_list(nullptr)
    , _render_target(nullptr)
{
    _draw_system.Prepare(_command_list);
    return;
}

DrawSystemFrame::~DrawSystemFrame()
{
    SetRenderTarget(nullptr);
    _draw_system.Present();
    return;
}

ID3D12GraphicsCommandList* DrawSystemFrame::GetCommandList()
{
    return _command_list;
}

const int DrawSystemFrame::GetBackBufferIndex()
{
    return _draw_system.GetBackBufferIndex();
}

void DrawSystemFrame::SetRenderTarget(
    IRenderTarget* const in_render_target, 
    const std::shared_ptr<IResource>& in_resource,
    const bool in_allow_clear
    )
{
    if (_render_target == in_render_target)
    {
        return;
    }

    if (_render_target)
    {
        _render_target->EndRender(_command_list);
    }

    _render_target = in_render_target;
    if (_render_target)
    {
        if (nullptr != in_resource)
        {
            _draw_system.AddFrameResource(in_resource);
        }
        _render_target->StartRender(_command_list, in_allow_clear);
    }
    return;
}

void DrawSystemFrame::SetShader(Shader* const in_shader)
{
    in_shader->SetActivate(
        _command_list,
        _draw_system.GetBackBufferIndex()
        );
    return;
}

void DrawSystemFrame::Draw(GeometryGeneric* const in_geometry)
{
    in_geometry->Draw(_command_list);
    return;
}

void DrawSystemFrame::Dispatch(
    uint32_t in_thread_group_count_x,
    uint32_t in_thread_group_count_y,
    uint32_t in_thread_group_count_z
    )
{
    _command_list->Dispatch(
        in_thread_group_count_x,
        in_thread_group_count_y,
        in_thread_group_count_z
        );
    return;
}

void DrawSystemFrame::ResourceBarrier(
    IResource* const in_resource,
    D3D12_RESOURCE_STATES in_after_state
    )
{
    in_resource->OnResourceBarrier(_command_list, in_after_state);
    return;
}

void DrawSystemFrame::UpdateGeometryGeneric(
    GeometryGeneric* const in_geometry,
    const std::vector<uint8_t>& in_vertex_data_raw
    )
{
    _draw_system.UpdateGeometryGeneric(
        _command_list,
        in_geometry,
        in_vertex_data_raw
        );
    return;
}

