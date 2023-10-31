#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_slider.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/ui/ui_data/ui_data_float.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_shader_enum.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_enum.h"

namespace
{
    const bool TweakLayout(
        IUIComponent* const in_component,
        const float in_value,
        const UIOrientation in_orientation
        )
    {
        if (nullptr == in_component)
        {
            return false;
        }
        UILayout layout = in_component->GetLayout();
        switch(in_orientation)
        {
        default:
            break;
        case UIOrientation::THorizontal:
            layout.SetSliderHorisontal(in_value);
            break;
        case UIOrientation::TVertical:
            layout.SetSliderVertical(in_value);
            break;
        }
        const bool dirty = in_component->SetLayout(layout);
        return dirty;
    }
}

UIComponentSlider::UIComponentSlider(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const UIOrientation in_orientation
    )
    : _content_default(
        in_base_colour,
        in_layout
        )
    , _orientation(in_orientation)
    , _child_data_knot()
    , _value_change()
    , _value(0.0f)
    , _range_low_high()
{
    auto geometry = std::make_unique<UIGeometry>();
    std::unique_ptr<IUIComponent> component;
    auto node = std::make_unique<UIHierarchyNode>();
    auto screen_space = std::make_unique<UIScreenSpace>();
    _child_data_knot = std::make_shared<UIHierarchyNodeChildData>(
        geometry,
        component,
        node,
        screen_space
        );
}

UIComponentSlider::~UIComponentSlider()
{
    // Nop
}

const bool UIComponentSlider::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const UIOrientation in_orientation
    )
{
    bool dirty = false;
    if (true == _content_default.SetBase(
        in_base_colour,
        in_layout
        ))
    {
        dirty = true;
    }
    if (_orientation != in_orientation)
    {
        _orientation = in_orientation;
        dirty = true;
    }
    return dirty;
}

const bool UIComponentSlider::Set(
    const std::function<void(const float)>& in_value_change,
    const float in_value,
    const VectorFloat2& in_range_low_high
    )
{
    bool dirty = false;

    _value_change = in_value_change;

    if (_value != in_value)
    {
        _value = in_value;
        dirty = true;
    }

    if (_range_low_high != in_range_low_high)
    {
        _range_low_high = in_range_low_high;
        dirty = true;
    }

    return dirty;
}

void UIComponentSlider::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentSlider::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIComponentSlider::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataFloat* const data = dynamic_cast<UIDataFloat*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetOnValueChange(),
            data->GetValue(),
            data->GetRangeLowHigh()
            ))
        {
            dirty = true;
        }
    }

    UIData* const knot_data = data ? data->GetKnotChildData() : nullptr;
    if (nullptr != knot_data)
    {
        auto factory = in_param._map_content_factory.find(knot_data->GetTemplateName());
        if (factory != in_param._map_content_factory.end())
        {
            UIComponentFactoryParam factory_param(
                in_param._draw_system,
                in_param._command_list,
                in_param._text_manager,
                0
                );
            if (true == factory->second(
                _child_data_knot->_component,
                factory_param
                ))
            {
                dirty = true;
                void* source_token = (void*)knot_data; //static_cast<void*>(&data); //reinterpret_cast<void*>(&data);
                _child_data_knot->_component->SetSourceToken(source_token);
                _child_data_knot->_node->MarkTextureDirty();
            }
        }
        else
        {
            if (nullptr != _child_data_knot->_component)
            {
                _child_data_knot->_component.reset();
                dirty = true;
            }
        }

        if (nullptr != _child_data_knot->_component)
        {
            if (true == _child_data_knot->_component->UpdateHierarchy(
                knot_data,
                *(_child_data_knot.get()),
                in_param
                ))
            {
                dirty = true;
                _child_data_knot->_node->MarkTextureDirty();
            }
        }
    }

    {
        const float domain = _range_low_high[1] - _range_low_high[0];
        const float value = domain != 0.0f ? (_value - _range_low_high[0]) / domain : 0.0f;
        if (true == TweakLayout(
            _child_data_knot->_component.get(), 
            value,
            _orientation
            ))
        {
            dirty = true;
        }
    }

    if (true == _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

void UIComponentSlider::UpdateSize(
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
        in_out_node,
        in_parent_screen_space,
        out_screen_space
        );

    if (nullptr != _child_data_knot->_component)
    {
        _child_data_knot->_component->UpdateSize(
            in_draw_system,
            in_parent_size,
            in_parent_offset,
            in_parent_window,
            in_ui_scale,
            in_time_delta, 
            *_child_data_knot->_geometry, 
            *_child_data_knot->_node,
            in_parent_screen_space,
            *_child_data_knot->_screen_space
            );
    }

    return;
}

void UIComponentSlider::GetDesiredSize(
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

//void UIComponentSlider::OnInputMouseClick(
//    const VectorFloat4&, // in_screen_pos,
//    const VectorFloat2& // in_mouse_pos
//    )
//{
//    return;
//}

const bool UIComponentSlider::Draw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    bool dirty = false;

    if ((nullptr != _child_data_knot) && (nullptr != _child_data_knot->_component))
    {
        if (true == _child_data_knot->_component->Draw(
            in_draw_param,
            *(_child_data_knot->_node.get())
            ))
        {
            dirty = true;
        }
    }

    if (true == in_node.PreDraw(
        in_draw_param
        ))
    {
        dirty = true;
    }
    if (true == in_node.Draw(
        in_draw_param,
        dirty
        ))
    {
        if (nullptr != _child_data_knot)
        {
            UIHierarchyNodeChildData& child_data = *_child_data_knot;
            const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

            child_data._node->GetUITexture().SetShaderResource(
                *shader,
                0,
                in_draw_param._frame
                );

            if (nullptr == child_data._shader_constant_buffer)
            {
                child_data._shader_constant_buffer = shader->MakeShaderConstantBuffer(
                    in_draw_param._draw_system
                    );
            }

            if (nullptr != child_data._shader_constant_buffer)
            {
                UIManager::TShaderConstantBuffer& buffer = child_data._shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
                // todo: get the tint colour, from geometry?
                buffer._tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f);
            }

            in_draw_param._frame->SetShader(shader, child_data._shader_constant_buffer);

            child_data._geometry->Draw(
                in_draw_param._draw_system,
                in_draw_param._frame
                );
        }

        dirty = true;
    }

    return dirty;

}
