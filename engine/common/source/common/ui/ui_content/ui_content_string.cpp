#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_string.h"

//#include "common/draw_system/draw_system_frame.h"
//#include "common/draw_system/draw_system.h"
#include "common/math/math.h"
#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"


UIContentString::UIContentString(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    std::unique_ptr<TextBlock>& in_text_block
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
    , _text_block(std::move(in_text_block))
{
    return;
}

UIContentString::~UIContentString()
{
    return;
}

const bool UIContentString::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    if (true == _content_default.SetBase(
        in_clear_background,
        in_clear_colour,
        in_layout
        ))
    {
        return true;
    }
    return false;
}

const bool UIContentString::Set(
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
void UIContentString::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentString::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentString::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentString::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    IUIData* const in_data,
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

void UIContentString::UpdateSize(
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
        in_out_node
        );

    // Return true if needed? no, if the size of the texture changes, then texture is marked as needs to draw
    _text_block->SetTextContainerSize(
        in_out_node.GetTextureSize(in_draw_system)
        );

    return;
}

void UIContentString::GetDesiredSize(
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

void UIContentString::PreDraw(
    const UIManagerDrawParam& in_param
    )
{
#if 1
    in_param._text_manager->DrawText(
        in_param._draw_system,
        in_param._frame,
        _text_block.get()
        );
#endif
    //_pre_draw_dirty = false;
    return;
}
