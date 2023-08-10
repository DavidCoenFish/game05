#pragma once

#include "common/ui/i_ui_content.h"

class TextBlock;

class UIContentText : public IUIContent
{
public:
    UIContentText(
        std::shared_ptr<TextBlock>& in_text_block
        );
    ~UIContentText();

private:
    //TextManager
    std::shared_ptr<TextBlock> _text_block;

};
