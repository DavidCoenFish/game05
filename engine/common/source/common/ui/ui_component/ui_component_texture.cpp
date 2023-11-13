#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_enum.h"

UIComponentTexture::UIComponentTexture(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
    const std::shared_ptr<IResource>& in_shader_resource
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _shader_resource_view_handle(in_shader_resource_view_handle)
    , _shader_resource(in_shader_resource)
    , _dirty(false)
{
    _geometry = std::make_unique<UIGeometry>();
}

UIComponentTexture::~UIComponentTexture()
{
    // Nop
}

void UIComponentTexture::SetTexture(
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
    const std::shared_ptr<IResource>& in_shader_resource
    )
{
    _dirty = true;
    _shader_resource_view_handle = in_shader_resource_view_handle;
    _shader_resource = in_shader_resource;
}

const bool UIComponentTexture::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    dirty = TSuper::UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        );
    if (true == _dirty)
    {
        dirty = true;
        _dirty = false;
    }
    return dirty;
}

const bool UIComponentTexture::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    bool dirty = false;
    auto& texture = in_node.GetUITexture();
    if ((false == texture.GetHasDrawn()) ||
        (true == texture.GetAlwaysDirty())
        )
    {
        if (false == texture.SetRenderTarget(
            in_draw_param._draw_system,
            in_draw_param._frame
            ))
        {
            return dirty;
        }

        dirty = true;

        const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

        texture.SetShaderResource(
            *shader,
            0,
            in_draw_param._frame
            );

        if (nullptr == _shader_constant_buffer)
        {
            _shader_constant_buffer = shader->MakeShaderConstantBuffer(
                in_draw_param._draw_system
                );
        }
        if (nullptr != _shader_constant_buffer)
        {
            UIManager::TShaderConstantBuffer& buffer = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
            buffer._tint_colour = TSuper::GetTintColour();
        }

        in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

        _geometry->Draw(
            in_draw_param._draw_system,
            in_draw_param._frame
            );

        texture.SetHasDrawn(true);
    }

    return dirty;
}

// this Component already uses the tint colour in it's custom PreDraw
const VectorFloat4 UIComponentTexture::GetTintColour() const
{
    return VectorFloat4::s_white;
}
