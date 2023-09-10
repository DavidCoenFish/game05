#pragma once

#include "common/ui/ui_content/ui_content_default.h"
#include "common/text/text_enum.h"

class TextBlock;
class TextFont;

class UIContentString : public UIContentDefault
{
public:
    UIContentString(
        std::unique_ptr<TextBlock>& in_text_block
        );
    ~UIContentString();

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
/*
    virtual const bool Update(
        VectorInt2& out_texture_size,
        UIHierarchyNodeLayoutData& out_layout_data,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
        const VectorInt2& in_parent_size,
        const IUIData* const in_data,
        const std::string& in_model_key,
        const UIHierarchyNodeUpdateLayoutParam& in_param
        ) override;
*/
private:
    //TextFont* _font;
    //int _font_size;
    //TextEnum::HorizontalLineAlignment _horizontal;
    //TextEnum::VerticalBlockAlignment _vertical;

    std::unique_ptr<TextBlock> _text_block;

};
