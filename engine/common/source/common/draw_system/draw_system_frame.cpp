#include "common/common_pch.h"
#include "common/draw_system/draw_system_frame.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_resource_list.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/log/log.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem&in_draw_system) 
    : _draw_system(in_draw_system)
    , _command_list(nullptr)
    , _render_target(nullptr)
{
    //LOG_MESSAGE_RENDER("DrawSystemFrame start");

    _draw_system.Prepare(_command_list);
    _resource_list = _draw_system.MakeResourceList();
    DSC_ASSERT(nullptr != _resource_list, "Make resource list failed");
    return;
}

DrawSystemFrame::~DrawSystemFrame()
{
    SetRenderTarget(nullptr);
    _draw_system.Present();
    _draw_system.FinishResourceList(_resource_list);
    //LOG_MESSAGE_RENDER("DrawSystemFrame end");

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
        // backbuffer render resource is not a IResource, but RenderTargetTexture is, deal upstream
        //_resource_list->AddResource(_render_target);
        _render_target->StartRender(_command_list, in_allow_clear);
    }

    return;
}

void DrawSystemFrame::SetRenderTargetTexture(
    const std::shared_ptr<RenderTargetTexture>& in_render_target,
    const bool in_allow_clear
    )
{
    AddFrameResource(in_render_target);
    SetRenderTarget(in_render_target.get(), in_allow_clear);
    return;
}


void DrawSystemFrame::AddFrameResource(
    const std::shared_ptr<IResource>& in_resource
    )
{
    _resource_list->AddResource(in_resource);
    return;
}

void DrawSystemFrame::SetShader(
    const std::shared_ptr<Shader>& in_shader,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_constant_buffer
    )
{
    in_shader->SetActive(
        _command_list,
        in_shader_constant_buffer.get()
        );
    _resource_list->AddResource(in_shader);
    _resource_list->AddResource(in_shader_constant_buffer);

    return;
}

void DrawSystemFrame::Draw(
    const std::shared_ptr<GeometryGeneric>& in_geometry
    )
{
    in_geometry->Draw(_command_list);
    _resource_list->AddResource(in_geometry);

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
    const std::shared_ptr<GeometryGeneric>& in_geometry,
    const std::vector<uint8_t>& in_vertex_data_raw
    )
{
    _draw_system.UpdateGeometryGeneric(
        _command_list,
        in_geometry.get(),
        in_vertex_data_raw
        );
    _resource_list->AddResource(in_geometry);
    return;
}

