#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_string.h"

//#include "common/draw_system/draw_system_frame.h"
//#include "common/draw_system/draw_system.h"
#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"


UIContentString::UIContentString(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    std::unique_ptr<TextBlock>& in_text_block,
    const int in_font_size,
    const int in_new_line_height,
    const int in_em_size
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
    , _text_block(std::move(in_text_block))
    , _font_size(in_font_size)
    , _new_line_height(in_new_line_height)
    , _em_size(in_em_size)
    , _pre_draw_dirty(true)
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
        _pre_draw_dirty = true;
        return true;
    }
    return false;
}

const bool UIContentString::Set(
    TextFont& in_font, 
    const int in_font_size,
    const int in_new_line_height,
    const TextEnum::HorizontalLineAlignment in_horizontal, 
    const TextEnum::VerticalBlockAlignment in_vertical,
    const int in_em_size
    )
{
    _font_size = in_font_size;
    _new_line_height = in_new_line_height;
    _em_size = in_em_size;

    bool dirty = false;
    if (true == _text_block->SetFont(in_font, in_font_size, in_new_line_height))
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
    if (true == _text_block->SetEMSize(in_em_size))
    {
        dirty = true;
    }

    if (true == dirty)
    {
        _pre_draw_dirty = true;
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
    const IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
#if 0
    return _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        );
#else
    bool dirty = false;
    const UIDataString* const data_string = dynamic_cast<const UIDataString*>(in_data);
    if (nullptr != data_string)
    {
        if (true == _text_block->SetText(
            data_string->GetString(),
            in_param._text_manager->GetLocaleToken(data_string->GetLocale())
            ))
        {
            dirty = true;
            _pre_draw_dirty = true;
        }
    }

    return dirty;
#endif
}

void UIContentString::UpdateSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _text_block->SetWidthLimit(
        _text_block->GetWidthLimitEnabled(),
        in_parent_size[0]
        );
    _text_block->SetEMSize(static_cast<int>(round(_em_size * in_ui_scale)));
    _text_block->SetFont(
        *(_text_block->GetFont()),
        static_cast<int>(round(_font_size * in_ui_scale)),
        static_cast<int>(round(_new_line_height * in_ui_scale))
        );
    _text_block->SetTextContainerSize(
        in_parent_size
        );

    _content_default.UpdateSize(
        in_parent_size,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node
        );
}

const VectorInt2 UIContentString::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
#if 0
    return _content_default.GetDesiredSize(
        in_parent_size,
        in_ui_scale
        );
#else

    _text_block->SetWidthLimit(
        _text_block->GetWidthLimitEnabled(),
        in_parent_size[0]
        );

    _text_block->SetEMSize(static_cast<int>(round(_em_size * in_ui_scale)));
    _text_block->SetFont(
        *(_text_block->GetFont()),
        static_cast<int>(round(_font_size * in_ui_scale)),
        static_cast<int>(round(_new_line_height * in_ui_scale))
        );
    return _text_block->GetTextBounds();
#endif
}

const bool UIContentString::GetNeedsPreDraw() const
{
    return _pre_draw_dirty;
}

void UIContentString::PreDraw(
    const UIManagerDrawParam& in_param
    )
{
    in_param._text_manager->DrawText(
        in_param._draw_system,
        in_param._frame,
        _text_block.get()
        );
    _pre_draw_dirty = false;
}
