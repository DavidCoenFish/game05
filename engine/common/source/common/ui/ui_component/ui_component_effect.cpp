#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_effect.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"

UIComponentEffect::UIComponentEffect(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const UIEffectEnum in_type,
    const UICoord& in_coord_a,
    const UICoord& in_coord_b,
    const UICoord& in_coord_c,
    const UICoord& in_coord_d
    )
    : _content_default(
        in_base_colour,
        in_layout
        )
    , _type(in_type)
    , _coord_a(in_coord_a)
    , _coord_b(in_coord_b)
    , _coord_c(in_coord_c)
    , _coord_d(in_coord_d)
{
    _geometry = std::make_unique<UIGeometry>();
}

UIComponentEffect::~UIComponentEffect()
{
    // Nop
}

const bool UIComponentEffect::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    return _content_default.SetBase(
        in_base_colour,
        in_layout
        );
}

// return true if modified, else false
const bool UIComponentEffect::Set(
    const UIEffectEnum in_type,
    const UICoord& in_coord_a,
    const UICoord& in_coord_b,
    const UICoord& in_coord_c,
    const UICoord& in_coord_d
    )
{
    bool dirty = false;

    if (_type != in_type)
    {
        dirty = true;
        _type = in_type;
        _shader_constant_buffer.reset();
    }

    if (_coord_a != in_coord_a)
    {
        dirty = true;
        _coord_a = in_coord_a;
    }

    if (_coord_b != in_coord_b)
    {
        dirty = true;
        _coord_b = in_coord_b;
    }

    if (_coord_c != in_coord_c)
    {
        dirty = true;
        _coord_c = in_coord_c;
    }

    if (_coord_d != in_coord_d)
    {
        dirty = true;
        _coord_d = in_coord_d;
    }

    return dirty;
}

// Make sorting children easier
void UIComponentEffect::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentEffect::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIComponentEffect::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIComponentEffect::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    if (true == _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        ))
    {
        dirty = true;
    }

    if (nullptr == _shader_constant_buffer)
    {
        Shader* const shader = in_param._ui_manager->GetEffectShader(_type);
        if (nullptr != shader)
        {
            _shader_constant_buffer = shader->MakeShaderConstantBuffer(
                in_param._draw_system
                );
        }
    }

    return dirty;
}

void UIComponentEffect::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    _content_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        in_out_geometry, 
        in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        in_parent_screen_space,
        out_screen_space
        );

    if (nullptr != _shader_constant_buffer)
    {
        const VectorInt2 texture_size = in_out_node.GetTextureSize(in_draw_system);
        UIManager::TShaderConstantBuffer& constant_0 = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
        TShaderConstantBuffer& constant_1 = _shader_constant_buffer->GetConstant<TShaderConstantBuffer>(1);
        constant_0._tint_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f);

        VectorFloat2 texture_size_float(static_cast<float>(texture_size.GetX()), static_cast<float>(texture_size.GetY()));
        constant_1._data = VectorFloat4(
            _coord_a.Calculate(texture_size_float, in_ui_scale),
            _coord_b.Calculate(texture_size_float, in_ui_scale),
            _coord_c.Calculate(texture_size_float, in_ui_scale),
            _coord_d.Calculate(texture_size_float, in_ui_scale)
            );
        constant_1._width_height_iwidth_iheight = VectorFloat4(
            static_cast<float>(texture_size.GetX()),
            static_cast<float>(texture_size.GetY()),
            1.0f / static_cast<float>(texture_size.GetX()),
            1.0f / static_cast<float>(texture_size.GetY())
            );

        auto& child_data_array = in_out_node.GetChildData();
        if (0 != child_data_array.size())
        {
            UIHierarchyNodeChildData& child_data = *(child_data_array[0]);
            child_data._geometry->Get(
                constant_1._geometry_pos,
                constant_1._geometry_uv
                );
        }
    }

    return;
}

void UIComponentEffect::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return _content_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
}

const bool UIComponentEffect::Draw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    )
{
    bool dirty = false;

    if (true == in_node.PreDraw(
        in_draw_param
        ))
    {
        dirty = true;
    }

    UITexture& texture = in_node.GetUITexture();
    if ((true == dirty) ||
        (false == texture.GetHasDrawn()) ||
        (true == texture.GetAlwaysDirty())
        )
    {
        std::shared_ptr<IResource> frame_resource;
        auto* const render_target = texture.GetRenderTarget(
            in_draw_param._draw_system,
            frame_resource,
            in_draw_param._frame->GetCommandList()
            );
        if (nullptr == render_target)
        {
            return dirty;
        }

        dirty = true;
        in_draw_param._frame->SetRenderTarget(
            render_target, 
            frame_resource,
            texture.GetAllowClear()
            );

        auto& child_data_array = in_node.GetChildData();
        if (0 != child_data_array.size())
        {
            UIHierarchyNodeChildData& child_data = *(child_data_array[0]);
            Shader* const shader = in_draw_param._ui_manager->GetEffectShader(_type);

            shader->SetShaderResourceViewHandle(0, child_data._node->GetShaderResourceHeapWrapperItem());
            in_draw_param._frame->SetShader(shader, _shader_constant_buffer.get());

            GeometryGeneric* const geometry = _geometry->GetGeometry(
                in_draw_param._draw_system,
                in_draw_param._frame->GetCommandList()
                );

            in_draw_param._frame->Draw(geometry);
        }

        texture.SetHasDrawn(true);
    }

    return dirty;
}

