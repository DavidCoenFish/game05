#include "common/common_pch.h"
#include "common/ui/ui_content_string.h"

#include "common/text/text_block.h"

UIContentString::UIContentString(
    std::unique_ptr<TextBlock>& in_text_block
    )
    : _text_block(std::move(in_text_block))
{
    return;
}

UIContentString::~UIContentString()
{
    return;
}
