#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_string.h"

#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"


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

const bool UIContentString::SetFont(
    TextFont& in_font, 
    const int in_font_size,
    const int in_new_line_height
    )
{
    return _text_block->SetFont(in_font, in_font_size, in_new_line_height);
}

const bool UIContentString::SetAlignment(
    TextEnum::HorizontalLineAlignment in_horizontal, 
    TextEnum::VerticalBlockAlignment in_vertical
    )
{
    return (_text_block->SetHorizontalLineAlignment(in_horizontal) ||
        _text_block->SetVerticalBlockAlignment(in_vertical));
}

/*
const bool UIContentString::Update(
    VectorInt2& out_texture_size,
    UIHierarchyNodeLayoutData& out_layout_data,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
    const VectorInt2& in_parent_size,
    const IUIData* const in_data,
    const std::string& in_model_key,
    const UIHierarchyNodeUpdateLayoutParam& in_param
    )
{
    out_child_data_array.clear();

    //set textblock text from in_data
    const UIDataString* const data_string = dynamic_cast<const UIDataString*>(in_data);

    bool dirty = false;
    std::string data;
    if (nullptr != data_string)
    {
        data = data_string->GetString();
    }
    auto* locale = in_param._text_manager->GetLocaleToken(in_param._locale);
    if (true == _text_block->SetText(
        data,
        locale
        ))
    {
        dirty = true;
    }

    return dirty;
}
*/
