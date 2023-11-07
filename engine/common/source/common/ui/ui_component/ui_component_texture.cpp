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
#include "common/ui/ui_shader_enum.h"

UIComponentTexture::UIComponentTexture(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
    const std::shared_ptr<IResource>& in_shader_resource
    )
    : _component_default(
        in_base_colour,
        in_layout
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

const bool UIComponentTexture::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const UIStateFlag UIComponentTexture::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentTexture::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentTexture::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentTexture::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentTexture::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    dirty = _component_default.UpdateHierarchy(
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

void UIComponentTexture::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>&,
    UILayout* const in_layout_override
    )
{
    _component_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node,
        in_parent_screen_space,
        out_screen_space,
        in_layout_override
        );
}

void UIComponentTexture::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    UILayout* const in_layout_override
    )
{
    return _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        in_layout_override
        );
}

const bool UIComponentTexture::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
#if 0
    return _component_default.Draw(
        in_draw_param,
        in_node
        );
#else
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
            buffer._tint_colour = _component_default.GetTintColour();
        }

        in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

        _geometry->Draw(
            in_draw_param._draw_system,
            in_draw_param._frame
            );

        texture.SetHasDrawn(true);
    }

    return dirty;

#endif
}
