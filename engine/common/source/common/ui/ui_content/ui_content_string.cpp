#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_string.h"

#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"


UIContentString::UIContentString(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    std::unique_ptr<TextBlock>& in_text_block
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
    , _text_block(std::move(in_text_block))
{
    return;
}

UIContentString::~UIContentString()
{
    return;
}

const bool UIContentString::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    return _content_default.SetBase(
        in_clear_background,
        in_clear_colour,
        in_layout
        );
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
    const TextEnum::HorizontalLineAlignment in_horizontal, 
    const TextEnum::VerticalBlockAlignment in_vertical,
    const int in_em_size
    )
{
    bool dirty = false;
    if (true == _text_block->SetHorizontalLineAlignment(in_horizontal))
    {
        dirty = true;
    }
    if (true == _text_block->SetVerticalBlockAlignment(in_vertical))
    {
        dirty = true;
    }
    if (true == _text_block->SetEMSize(in_em_size))
    {
        dirty = true;
    }

    return dirty;
}

// Make sorting children easier
void UIContentString::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentString::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentString::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentString::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    const IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        );
}


void UIContentString::UpdateSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_parent_size,
        in_ui_scale, 
        in_out_geometry, 
        in_out_node
        );
}

const VectorInt2 UIContentString::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
    return _content_default.GetDesiredSize(
        in_parent_size,
        in_ui_scale
        );
}

void UIContentString::Draw(
    const UIManagerDrawParam& in_param,
    Shader* const in_shader,
    UIGeometry* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
    )
{
    _content_default.Draw(
        in_param,
        in_shader,
        in_geometry,
        in_heap_wrapper_item
        );
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
