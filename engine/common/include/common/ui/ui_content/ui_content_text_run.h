#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/text/text_enum.h"

class ITextRunData;
class TextRun;
class TextFont;

class UIContentTextRun : public IUIContent
{
public:
    UIContentTextRun(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout,
        std::unique_ptr<TextRun>& in_text_run
        );
    ~UIContentTextRun();

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

    const bool Set(
        const bool in_width_limit_enabled,
        const TextEnum::HorizontalLineAlignment in_horizontal,
        const TextEnum::VerticalBlockAlignment in_vertical,
        const int in_em_size
        );

private:
    // Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool SetLayout(const UILayout& in_layout) override;

    virtual const bool UpdateHierarchy(
        //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    virtual const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    //virtual const bool GetNeedsPreDraw() const override;
    virtual void PreDraw(
        const UIManagerDrawParam& in_param
        ) override;

private:
    UIContentDefault _content_default;

    int _change_id;
    std::unique_ptr<TextRun> _text_run;

};
