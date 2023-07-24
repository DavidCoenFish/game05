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
    _draw_system.Present();
}

ID3D12GraphicsCommandList* DrawSystemFrame::GetCommandList()
{
    return _command_list;
}

const int DrawSystemFrame::GetBackBufferIndex()
{
    return _draw_system.GetBackBufferIndex();
}

void DrawSystemFrame::SetRenderTarget(IRenderTarget* const in_render_target)
{
    if (_render_target)
    {
        _render_target->EndRender(_command_list);
    }
    _render_target = in_render_target;
    if (_render_target)
    {
        _render_target->StartRender(_command_list);
    }
}

void DrawSystemFrame::SetShader(Shader* const in_shader)
{
    in_shader->SetActivate(
        _command_list,
        _draw_system.GetBackBufferIndex()
        );
}

void DrawSystemFrame::Draw(GeometryGeneric* const in_geometry)
{
    in_geometry->Draw(_command_list);
}

void DrawSystemFrame::Dispatch(
    uint32_t in_num_groups_x,
    uint32_t in_num_groups_y,
    uint32_t in_num_groups_z
    )
{
    _command_list->Dispatch(
        in_num_groups_x,
        in_num_groups_y,
        in_num_groups_z
        );
}

void DrawSystemFrame::ResourceBarrier(
    IResource* const in_resource,
    D3D12_RESOURCE_STATES in_after_state
    )
{
    in_resource->OnResourceBarrier(_command_list, in_after_state);
}

