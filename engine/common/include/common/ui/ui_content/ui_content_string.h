#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/text/text_enum.h"

class TextBlock;
class TextFont;

class UIContentString : public IUIContent
{
public:
    UIContentString(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout,
        std::unique_ptr<TextBlock>& in_text_block,
        const int in_font_size,
        const float in_new_line_gap_ratio
        );
    ~UIContentString();

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

    // return true if we have a text block which this is a new value, else true
    const bool Set(
        TextFont& in_font, 
        const int in_font_size,
        const float in_new_line_gap_ratio,
        const TextEnum::HorizontalLineAlignment in_horizontal, 
        const TextEnum::VerticalBlockAlignment in_vertical
        );

private:
    // Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool SetLayout(const UILayout& in_layout) override;

    virtual const bool UpdateHierarchy(
        //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
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

    virtual const bool GetNeedsPreDraw() const override;
    virtual void PreDraw(
        const UIManagerDrawParam& in_param
        ) override;

private:
    UIContentDefault _content_default;

    //bool _use_parent_size_for_width_limit;
    std::unique_ptr<TextBlock> _text_block;

    // these interact with UI scale, so we need a copy
    int _font_size;
    float _new_line_gap_ratio;
    bool _pre_draw_dirty;

};
