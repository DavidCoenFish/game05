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
    IUIData* const in_data,
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
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node
        );

    _text_run->SetTextContainerSize(
        in_out_node.GetTextureSize(in_draw_system)
        );
}

const VectorInt2 UIContentTextRun::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _text_run->SetWidthLimitWidth(in_parent_size[0]);
    _text_run->SetUIScale(in_ui_scale);

    return _text_run->GetTextBounds();
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
