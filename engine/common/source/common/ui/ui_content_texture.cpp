#include "common/common_pch.h"
#include "common/ui/ui_content_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_geometry.h"

UIContentTexture::UIContentTexture(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
    )
    : _shader_resource_view_handle()
    , _dirty(false)
{
    SetTexture(in_shader_resource_view_handle);
    _geometry = std::make_shared<UIGeometry>(
        in_draw_system,
        in_command_list
        );
}

UIContentTexture::~UIContentTexture()
{
    // Nop
}

void UIContentTexture::Draw(
    DrawSystemFrame* const in_frame,
    UITexture* const in_texture,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>&,
    Shader* const in_shader,
    const bool in_allow_clear
    )
{
    in_frame->SetRenderTarget(in_texture->GetRenderTarget(), in_allow_clear);
    in_shader->SetShaderResourceViewHandle(
        0,
        _shader_resource_view_handle
    );
    in_frame->SetShader(in_shader);
    in_frame->Draw(_geometry->GetGeometry());
    _dirty = false;

    return;
}

const bool UIContentTexture::GetNeedsToDraw() const
{
    return _dirty;
}

void UIContentTexture::SetTexture(
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
    )
{
    _dirty = true;
    _shader_resource_view_handle = in_shader_resource_view_handle;
}

