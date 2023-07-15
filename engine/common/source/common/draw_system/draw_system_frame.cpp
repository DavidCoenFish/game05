#include "common/common_pch.h"

#include "common/draw_system/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem&in_draw_system) 
    : draw_system(in_draw_system)
    , command_list(nullptr)
    , render_target(nullptr)
{
    draw_system.Prepare(command_list);
    // M_drawSystem.Clear();
}

DrawSystemFrame::~DrawSystemFrame()
{
    // If (m_pCommandList)
    // {
    // DX::ThrowIfFailed(m_pCommandList->Close());
    // }
    // If (m_pCommandList && m_pCommandQueue)
    // {
    // M_pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));
    // }
    SetRenderTarget(nullptr);
    draw_system.Present();
}

ID3D12GraphicsCommandList* DrawSystemFrame::GetCommandList()
{
    return command_list;
}

const int DrawSystemFrame::GetBackBufferIndex()
{
    return draw_system.GetBackBufferIndex();
}

void DrawSystemFrame::SetRenderTarget(IRenderTarget* const in_render_target)
{
    if (render_target)
    {
        render_target->EndRender(in_command_list);
    }
    render_target = in_render_target;
    if (render_target)
    {
        render_target->StartRender(in_command_list);
    }
}

void DrawSystemFrame::SetShader(Shader* const in_shader)
{
    in_shader->SetActivate(
        in_command_list,
        in_draw_system.GetBackBufferIndex()
        );
}

void DrawSystemFrame::Draw(GeometryGeneric* const in_geometry)
{
    in_geometry->Draw(in_command_list);
}

void DrawSystemFrame::Dispatch(
    uint32_t in_num_groups_x,
    uint32_t in_num_groups_y,
    uint32_t in_num_groups_z
    )
{
    command_list->Dispatch(
        in_num_groups_x,
        in_num_groups_y,
        in_num_groups_z
        );
}

