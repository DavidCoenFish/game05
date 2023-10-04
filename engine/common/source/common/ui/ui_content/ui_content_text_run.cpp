#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_text_run.h"

#include "common/text/text_run.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"


UIContentTextRun::UIContentTextRun(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    std::unique_ptr<TextRun>& in_text_run
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
    , _text_run(std::move(in_text_run))
    , _change_id(0)
{
    return;
}

UIContentTextRun::~UIContentTextRun()
{
    return;
}

const bool UIContentTextRun::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    bool dirty = false;
    if (true == _content_default.SetBase(
        in_clear_background,
        in_clear_colour,
        in_layout
        ))
    {
        dirty = true;
    }
    return dirty;
}

const bool UIContentTextRun::Set(
    const bool in_width_limit_enabled,
    const TextEnum::HorizontalLineAlignment in_horizontal,
    const TextEnum::VerticalBlockAlignment in_vertical,
    const int in_em_size
    )
{
    bool dirty = false;
    if (true == _text_run->SetEMSize(in_em_size))
    {
        dirty = true;
    }
    if (true == _text_run->SetWidthLimitEnabled(in_width_limit_enabled))
    {
        dirty = true;
    }
    if (true == _text_run->SetHorizontalLineAlignment(in_horizontal))
    {
        dirty = true;
    }
    if (true == _text_run->SetVerticalBlockAlignment(in_vertical))
    {
        dirty = true;
    }

    return dirty;
}


// Make sorting children easier
void UIContentTextRun::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentTextRun::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentTextRun::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentTextRun::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataTextRun* const data_text_run = dynamic_cast<UIDataTextRun*>(in_data);
    if (nullptr != data_text_run)
    {
        if (true == data_text_run->VisitData(
            *(in_param._text_manager),
            *(in_param._locale_system),
            *(in_param._default_text_style),
            [this](const int in_change_id, const std::vector<std::shared_ptr<ITextRunData>>& in_data_array)->bool{
                if (_change_id != in_change_id)
                {
                    _change_id = in_change_id;
                    _text_run->SetTextRunArray(in_data_array);
                    return true;
                }
                return false;
            }))
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

void UIContentTextRun::UpdateSize(
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

    _text_run->SetTextContainerSize(
        in_out_node.GetTextureSize(in_draw_system)
        );
}

void UIContentTextRun::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    out_layout_size = _content_default.GetLayout().GetSize(in_parent_window, in_ui_scale);

    _text_run->SetWidthLimitWidth(out_layout_size[0]);
    _text_run->SetUIScale(in_ui_scale);

    out_desired_size = _text_run->GetTextBounds();

    out_layout_size = _content_default.GetLayout().CalculateShrinkSize(out_layout_size, out_desired_size);
    //out_desired_size = VectorInt2::Max(out_layout_size, out_desired_size);

    return;
}

//const bool UIContentTextRun::GetNeedsPreDraw() const
//{
//    return _pre_draw_dirty;
//}

void UIContentTextRun::PreDraw(
    const UIManagerDrawParam& in_param
    )
{
#if 1
    in_param._text_manager->DrawTextRun(
        in_param._draw_system,
        in_param._frame,
        _text_run.get()
        );
#endif
    return;
}
