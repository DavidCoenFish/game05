#pragma once

#include "common/ui/i_ui_content.h"
#include "common/text/text_enum.h"

class TextBlock;
class TextFont;

class UIContentString : public IUIContent
{
public:
    UIContentString(
        std::unique_ptr<TextBlock>& in_text_block
        );
    ~UIContentString();

    // return true if we have a text block which this is a new value, else true
    const bool SetFont(
        TextFont& in_font, 
        const int in_font_size
        );

    const bool SetAlignment(
        TextEnum::HorizontalLineAlignment in_horizontal, 
        TextEnum::VerticalBlockAlignment in_vertical
        );

private:
    virtual const bool Update(
        VectorInt2& out_texture_size,
        UIHierarchyNodeLayoutData& out_layout_data,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
        const VectorInt2& in_parent_size,
        const IUIData* const in_data,
        const std::string& in_model_key,
        const UIHierarchyNodeUpdateLayoutParam& in_param
        ) override;

private:
    //TextFont* _font;
    //int _font_size;
    //TextEnum::HorizontalLineAlignment _horizontal;
    //TextEnum::VerticalBlockAlignment _vertical;

    std::unique_ptr<TextBlock> _text_block;

};
