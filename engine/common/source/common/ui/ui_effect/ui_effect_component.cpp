#include "common/common_pch.h"
#include "common/ui/ui_effect/ui_effect_component.h"

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
		case UIEffectEnum::TDebugGrid:
			result = UIShaderEnum::TEffectDebugGrid;
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

	const VectorFloat4 CalculateTintColour(
		const UIStateFlag in_state_flag, 
		const std::array<VectorFloat4, static_cast<int>(UIStateFlag::TTintPermutationCount)>* const in_state_flag_tint_array_or_null,
		const VectorFloat4& in_default_tint
		)
	{
		if (nullptr == in_state_flag_tint_array_or_null)
		{
			return in_default_tint; //VectorFloat4::s_white;
		}

		const int index = static_cast<int>(in_state_flag) & static_cast<int>(UIStateFlag::TTintMask);
		return (*in_state_flag_tint_array_or_null)[index];
	}
}

UIEffectComponent::UIEffectComponent(
	const UIEffectEnum in_type,
	const UICoord& in_coord_a,
	const UICoord& in_coord_b,
	const UICoord& in_coord_c,
	const UICoord& in_coord_d,
	const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null,
	const VectorFloat4& in_default_tint
	)
	: _state_flag_tint_array_or_null(in_state_flag_tint_array_or_null)
	, _type(in_type)
	, _coord_a(in_coord_a)
	, _coord_b(in_coord_b)
	, _coord_c(in_coord_c)
	, _coord_d(in_coord_d)
	, _default_tint(in_default_tint)
{
	_texture = std::make_unique<UITexture>(
		true, //const bool in_draw_to_texture = false,
		false, //const bool in_always_dirty = false,
		true, //const bool in_allow_clear = false,
		VectorFloat4::s_zero //const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
		);
}

UIEffectComponent::~UIEffectComponent()
{
	// Nop
}

// return true if modified, else false
const bool UIEffectComponent::Set(
	const UIEffectEnum in_type,
	const UICoord& in_coord_a,
	const UICoord& in_coord_b,
	const UICoord& in_coord_c,
	const UICoord& in_coord_d,
	const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null,
	const VectorFloat4& in_default_tint
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

	if (_state_flag_tint_array_or_null != in_state_flag_tint_array_or_null)
	{
		dirty = true;
		_state_flag_tint_array_or_null = in_state_flag_tint_array_or_null;
	}

	if (_default_tint != in_default_tint)
	{
		dirty = true;
		_default_tint = in_default_tint;
	}

	return dirty;
}

void UIEffectComponent::Update(
	const UIHierarchyNodeUpdateParam& in_param,
	const VectorInt2& in_target_size,
	const VectorInt4& in_texture_margin
	)
{
	DSC_ASSERT(nullptr != _texture, "texture should be created in ctor");
	_texture->SetSize(in_target_size);
	_margin = VectorFloat4(
		in_texture_margin[0] * in_param._ui_scale,
		in_texture_margin[1] * in_param._ui_scale,
		in_texture_margin[2] * in_param._ui_scale,
		in_texture_margin[3] * in_param._ui_scale
		);
	return;
}

void UIEffectComponent::Render(
	const UIManagerDrawParam& in_draw_param,
	UITexture& in_input_texture,
	const UIStateFlag in_state_flag
	)
{
	if (false == _texture->SetRenderTarget(in_draw_param._draw_system, in_draw_param._frame))
	{
		DSC_ASSERT_ALWAYS("why did the effect render target fail");
		return;
	}

	const UIShaderEnum shader_type = GetShaderType(_type);
	const auto& shader = in_draw_param._ui_manager->GetShaderRef(shader_type);

	if (false == in_input_texture.SetShaderResource(
		*shader,
		0,
		in_draw_param._frame,
		in_draw_param._draw_system
		))
	{
		DSC_ASSERT_ALWAYS("why did the shader resource fail");
		return;
	}

	if (nullptr == _shader_constant_buffer)
	{
		_shader_constant_buffer = shader->MakeShaderConstantBuffer(
			in_draw_param._draw_system
			);
	}

	if (nullptr != _shader_constant_buffer)
	{
		TShaderConstantBuffer& buffer = _shader_constant_buffer->GetConstant<TShaderConstantBuffer>(0);
		const VectorInt2 texture_size = _texture->GetSize(in_draw_param._draw_system);
		VectorFloat2 texture_size_float(static_cast<float>(texture_size.GetX()), static_cast<float>(texture_size.GetY()));

		buffer._data = VectorFloat4(
			_coord_a.Calculate(texture_size_float, in_draw_param._ui_scale),
			_coord_b.Calculate(texture_size_float, in_draw_param._ui_scale),
			_coord_c.Calculate(texture_size_float, in_draw_param._ui_scale),
			_coord_d.Calculate(texture_size_float, in_draw_param._ui_scale)
			);

		buffer._width_height_iwidth_iheight = VectorFloat4(
			texture_size_float.GetX(),
			texture_size_float.GetY(),
			1.0f / texture_size_float.GetX(),
			1.0f / texture_size_float.GetY()
			);
	
		buffer._tint = CalculateTintColour(in_state_flag, _state_flag_tint_array_or_null.get(), _default_tint);
		buffer._margin = _margin;
	}

	in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

	in_draw_param._frame->Draw(
		in_draw_param._ui_manager->GetEffectGeometryRef()
		);

	return;
}
