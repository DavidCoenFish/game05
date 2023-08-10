#include "common/common_pch.h"
#include "common/ui/ui_content_text.h"

#include "common/text/text_block.h"

UIContentText::UIContentText(
    std::shared_ptr<TextBlock>& in_text_block
    )
    : _text_block(in_text_block)
{
    return;
}

UIContentText::~UIContentText()
{
    return;
}
