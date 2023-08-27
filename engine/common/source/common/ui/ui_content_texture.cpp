#include "common/common_pch.h"
#include "common/ui/ui_content_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"

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
    const UIManagerDrawParam& in_param,
    UITexture* const in_texture,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>&,// in_child_data_array, // Todo, a ui_content which draws the texture of each child?
    Shader* const in_shader
    )
{
    in_param._frame->SetRenderTarget(
        in_texture->GetRenderTarget(), 
        in_texture->GetAllowClear()
        );
    in_shader->SetShaderResourceViewHandle(
        0,
        _shader_resource_view_handle
    );
    in_param._frame->SetShader(in_shader);
    in_param._frame->Draw(_geometry->GetGeometry());

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

