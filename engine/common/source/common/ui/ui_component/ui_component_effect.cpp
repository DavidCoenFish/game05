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
#include "common/ui/ui_enum.h"
#include "common/ui/ui_enum.h"

#include "common/log/log.h"

//        LOG_CONSOLE("SetStateFlag %d => %d", _state_flag, in_state_flag);

namespace
{
    const UIShaderEnum GetShaderType(const UIEffectEnum in_type)
    {
        UIShaderEnum result = UIShaderEnum::TCount;
        switch(in_type)
        {
        default:
            DSC_ASSERT_ALWAYS("missing switch case effect shader");
            break;
        case UIEffectEnum::TDebug:
            result = UIShaderEnum::TEffectDebug;
            break;
        case UIEffectEnum::TDropShadow:
            result = UIShaderEnum::TEffectDropShadow;
            break;
        case UIEffectEnum::TInnerShadow:
            result = UIShaderEnum::TEffectInnerShadow;
            break;
        case UIEffectEnum::TRoundCorners:
            result = UIShaderEnum::TEffectRoundCorners;
            break;
        case UIEffectEnum::TRoundCornersHollow:
            result = UIShaderEnum::TEffectRoundCornersHollow;
            break;
        case UIEffectEnum::TGloss:
            result = UIShaderEnum::TEffectGloss;
            break;
        case UIEffectEnum::TFill:
            result = UIShaderEnum::TEffectFill;
            break;
        }

        return result;
    }
}

UIComponentEffect::UIComponentEffect(
    const UIEffectEnum in_type,
    const UICoord& in_coord_a,
    const UICoord& in_coord_b,
    const UICoord& in_coord_c,
    const UICoord& in_coord_d,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_array_or_null
    )
    : IUIComponent(in_state_flag_array_or_null)
    , _type(in_type)
    , _coord_a(in_coord_a)
    , _coord_b(in_coord_b)
    , _coord_c(in_coord_c)
    , _coord_d(in_coord_d)
{
#if defined(GEOMETRY_SIZE_INTO_SHADER)
    _geometry = std::make_unique<UIGeometry>();
#endif
}

UIComponentEffect::~UIComponentEffect()
{
    // Nop
}

// return true if modified, else false
const bool UIComponentEffect::Set(
    const UIEffectEnum in_type,
    const UICoord& in_coord_a,
    const UICoord& in_coord_b,
    const UICoord& in_coord_c,
    const UICoord& in_coord_d,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_array_or_null
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

    if (true == SetStateFlagArrayOrNull(in_state_flag_array_or_null))
    {
        dirty = true;
    }

    return dirty;
}

/*
const bool UIComponentEffect::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    if (true == TSuper::UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        ))
    {
        dirty = true;
    }

    if (nullptr == _shader_constant_buffer)
    {
        const auto shader_type = GetShaderType(_type);
        const Shader* const shader = in_param._ui_manager->GetShaderRef(shader_type).get();
        if (nullptr != shader)
        {
            _shader_constant_buffer = shader->MakeShaderConstantBuffer(
                in_param._draw_system
                );
        }
    }

    return dirty;
}

const bool UIComponentEffect::UpdateSize(
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
    bool dirty = TSuper::UpdateSize(
        in_draw_system,
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
*/

const bool UIComponentEffect::SetContainerSize(
    const VectorInt2& in_texture_size,
    const UIHierarchyNodeUpdateLayoutRenderParam& in_param
    )
{
    bool dirty = false;
    if (nullptr == _shader_constant_buffer)
    {
        const auto shader_type = GetShaderType(_type);
        const Shader* const shader = in_param._ui_manager->GetShaderRef(shader_type).get();
        if (nullptr != shader)
        {
            _shader_constant_buffer = shader->MakeShaderConstantBuffer(
                in_param._draw_system
                );
        }
    }

    if (nullptr != _shader_constant_buffer)
    {
        TShaderConstantBuffer& constant_1 = _shader_constant_buffer->GetConstant<TShaderConstantBuffer>(1);
        if (UIEffectEnum::TNone != _type)
        {
            VectorFloat2 texture_size_float(static_cast<float>(in_texture_size.GetX()), static_cast<float>(in_texture_size.GetY()));

            const VectorFloat4 data(
                _coord_a.Calculate(texture_size_float, in_param._ui_scale),
                _coord_b.Calculate(texture_size_float, in_param._ui_scale),
                _coord_c.Calculate(texture_size_float, in_param._ui_scale),
                _coord_d.Calculate(texture_size_float, in_param._ui_scale)
                );
            if (constant_1._data != data)
            {
                dirty = true;
                constant_1._data = data;
            }

            const VectorFloat4 width_height_iwidth_iheight(
                texture_size_float.GetX(),
                texture_size_float.GetY(),
                1.0f / texture_size_float.GetX(),
                1.0f / texture_size_float.GetY()
                );
            if (constant_1._width_height_iwidth_iheight != width_height_iwidth_iheight)
            {
                dirty = true;
                constant_1._width_height_iwidth_iheight = width_height_iwidth_iheight;
            }
        }
    }

    return dirty;
}

const bool UIComponentEffect::PreDraw(
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

    // Rather than put this in Component::Draw(), as we dont call the component_default::PreDraw which calls Node::Draw and Component::Draw
    UITexture& texture = in_node.GetUITexture();
    if ((true == dirty) ||
        (false == texture.GetHasDrawn()) ||
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

        auto& child_data_array = in_node.GetChildData();
        if (0 != child_data_array.size())
        {
            UIHierarchyNodeChildData& child_data = *(child_data_array[0]);
            const auto shader_type = GetShaderType(_type);
            const auto& shader = in_draw_param._ui_manager->GetShaderRef(shader_type);

            child_data._node->GetUITexture().SetShaderResource(
                *shader,
                0,
                in_draw_param._frame
                );

            // the tint array index may be changed by input after update size and before draw
            if (nullptr != _shader_constant_buffer)
            {
                UIManager::TShaderConstantBuffer& constant_0 = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
                const auto new_tint = GetTintColour();
                if (constant_0._tint_colour != new_tint)
                {
                    constant_0._tint_colour = new_tint;
                }
            }

            in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

#if defined(GEOMETRY_SIZE_INTO_SHADER)
            _geometry->Draw(
                in_draw_param._draw_system,
                in_draw_param._frame
                );
#else

#if defined(_DEBUG)
            VectorFloat4 geometry_pos;
            VectorFloat4 geometry_uv;
            child_data._geometry->Get(
                geometry_pos,
                geometry_uv
                );
            // well, till fixing layout model, shrinking the UIScale < 1.0 seems to break effect/ text is wrapping in a post shrunk container that should have been correct size to avoid wrap?
            DSC_ASSERT(geometry_pos == VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f), "Expect child geometry to be full screen");
            // atention Y inverted
            DSC_ASSERT(geometry_uv == VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f), "Expect child geometry to be full screen uv"); 
#endif

            child_data._geometry->Draw(
                in_draw_param._draw_system,
                in_draw_param._frame
                );
#endif
        }

        texture.SetHasDrawn(true);
    }

    return dirty;
}

