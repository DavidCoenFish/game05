#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"

UIContentTexture::UIContentTexture(
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
    )
    : _shader_resource_view_handle()
    , _dirty(false)
{
    SetTexture(in_shader_resource_view_handle);
    _geometry = std::make_shared<UIGeometry>();
}

UIContentTexture::~UIContentTexture()
{
    // Nop
}

const int UIContentTexture::GetClassTypeID() const
{
    return ClassTypeID<UIContentTexture>();
}

void UIContentTexture::Draw(
    const UIManagerDrawParam& in_param,
    UITexture* const in_texture,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>&,// in_child_data_array, // Todo, a ui_content which draws the texture of each child?
    Shader* const in_shader
    )
{
    auto* render_target = in_texture->GetRenderTarget(
        in_param._draw_system,
        in_param._frame->GetCommandList()
        );
    auto* geometry = _geometry->GetGeometry(
        in_param._draw_system,
        in_param._frame->GetCommandList()
        );
    in_param._frame->SetRenderTarget(
        render_target, 
        in_texture->GetAllowClear()
        );
    in_shader->SetShaderResourceViewHandle(
        0,
        _shader_resource_view_handle
        );
    in_param._frame->SetShader(in_shader);
    in_param._frame->Draw(geometry);

    _dirty = false;
    //SetDrawn();

    return;
}

const bool UIContentTexture::GetNeedsDraw() const
{
    return _dirty;
}

//void UIContentTexture::SetDrawn()
//{
//    _dirty = false;
//}

void UIContentTexture::SetTexture(
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
    )
{
    _dirty = true;
    _shader_resource_view_handle = in_shader_resource_view_handle;
}

