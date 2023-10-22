#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_string.h"

#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/draw_system.h"
#include "common/math/dsc_math.h"
#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"


UIComponentString::UIComponentString(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    std::unique_ptr<TextBlock>& in_text_block
    )
    : _content_default(
        in_base_colour,
        in_layout
        )
    , _text_block(std::move(in_text_block))
{
    return;
}

UIComponentString::~UIComponentString()
{
    return;
}

const bool UIComponentString::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    if (true == _content_default.SetBase(
        in_base_colour,
        in_layout
        ))
    {
        return true;
    }
    return false;
}

const bool UIComponentString::Set(
    TextFont& in_font, 
    const int in_font_size,
    const float in_new_line_gap_ratio,
    const bool in_width_limit_enabled,
    const TextEnum::HorizontalLineAlignment in_horizontal, 
    const TextEnum::VerticalBlockAlignment in_vertical,
    const VectorFloat4& in_text_colour
    )
{
    bool dirty = false;
    if (true == _text_block->SetFont(in_font))
    {
        dirty = true;
    }
    if (true == _text_block->SetFontSize(in_font_size))
    {
        dirty = true;
    }
    if (true == _text_block->SetNewLineGapRatio(in_new_line_gap_ratio))
    {
        dirty = true;
    }
    if (true == _text_block->SetWidthLimitEnabled(in_width_limit_enabled))
    {
        dirty = true;
    }
    if (true == _text_block->SetHorizontalLineAlignment(in_horizontal))
    {
        dirty = true;
    }
    if (true == _text_block->SetVerticalBlockAlignment(in_vertical))
    {
        dirty = true;
    }
    if (true == _text_block->SetColour(in_text_colour))
    {
        dirty = true;
    }

    return dirty;
}

// Make sorting children easier
void UIComponentString::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentString::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

//const bool UIComponentString::SetLayout(const UILayout& in_layout)
//{
//    return _content_default.SetLayout(in_layout);
//}

const bool UIComponentString::UpdateHierarchy(
    //std::vector<std::shared_ptr<UIData>>*& out_array_data_or_null,
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataString* const data_string = dynamic_cast<const UIDataString*>(in_data);
    if (nullptr != data_string)
    {
        if (true == _text_block->SetText(
            data_string->GetStringRef(),
            in_param._text_manager->GetLocaleToken(data_string->GetLocale())
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

void UIComponentString::UpdateSize(
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

    // Return true if needed? no, if the size of the texture changes, then texture is marked as needs to draw
    _text_block->SetTextContainerSize(
        in_out_node.GetTextureSize(in_draw_system)
        );

    return;
}

void UIComponentString::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    out_layout_size = _content_default.GetLayout().GetSize(in_parent_window, in_ui_scale);

    _text_block->SetWidthLimit(
        _text_block->GetWidthLimitEnabled(),
        out_layout_size[0]
        );
    _text_block->SetUIScale(in_ui_scale);

    out_desired_size = _text_block->GetTextBounds();

    out_layout_size = _content_default.GetLayout().CalculateShrinkSize(out_layout_size, out_desired_size);
    //out_desired_size = VectorInt2::Max(out_layout_size, out_desired_size);

    return;
}

const bool UIComponentString::Draw(
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

        in_draw_param._text_manager->DrawText(
            in_draw_param._draw_system,
            in_draw_param._frame,
            _text_block.get()
            );

        texture.SetHasDrawn(true);
    }

    return dirty;
}
