#pragma once

#include "common/ui/i_ui_content.h"
#include "common/text/text_enum.h"

class TextBlock;

class UIContentString : public IUIContent
{
public:
    UIContentString(
        std::unique_ptr<TextBlock>& in_text_block
        );
    ~UIContentString();

    // return true if we have a text block which this is a new value, else true
    const bool SetFont(
        const std::filesystem::path& in_font_rel_path, 
        const int in_font_size
        );

    const bool SetAlignment(
        TextEnum::HorizontalLineAlignment in_horizontal, 
        TextEnum::VerticalBlockAlignment in_vertical
        );

private:
    //TextManager
    std::unique_ptr<TextBlock> _text_block;

};
