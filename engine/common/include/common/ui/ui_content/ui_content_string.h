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
        std::unique_ptr<TextBlock>& in_text_block
        );
    ~UIContentString();

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

    // return true if we have a text block which this is a new value, else true
    const bool SetFont(
        TextFont& in_font, 
        const int in_font_size,
        const int in_new_line_height
        );

    const bool SetAlignment(
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
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    virtual const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        ) override;

    virtual void Draw(
        const UIManagerDrawParam& in_param,
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        ) override;

private:
    UIContentDefault _content_default;

    //bool _use_parent_size_for_width_limit;
    std::unique_ptr<TextBlock> _text_block;

};
