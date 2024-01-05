#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/text/text_enum.h"
#include "common/text/text_run.h"

class ITextRunData;
class TextRun;
class TextFont;

class UIComponentTextRun : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentTextRun(
        std::unique_ptr<TextRun>& in_text_run
        );
    ~UIComponentTextRun();

    const bool Set(
        const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
        const bool in_width_limit_enabled,
        const TextEnum::HorizontalLineAlignment in_horizontal,
        const TextEnum::VerticalBlockAlignment in_vertical,
        const int in_em_size,
        const float in_ui_scale
        );

private:
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_pre_shrink_layout_size //in_parent_window
        ) override;

    virtual void UpdateResources(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size,
        const VectorInt2& in_parent_window,
        const VectorInt2& in_parent_offset,
        const VectorInt4& in_texture_margin,
        const VectorInt2& in_parent_adjust_offset
        ) override;

    virtual void PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

private:
    //int _change_id;
    std::unique_ptr<TextRun> _text_run;

};
