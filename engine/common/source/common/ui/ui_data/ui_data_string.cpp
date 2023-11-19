#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_string.h"

#include "common/ui/ui_component/ui_component_string.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/text/text_manager.h"
#include "common/text/text_block.h"

UIDataString::UIDataString(
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null,
    const std::string& in_data,
    const LocaleISO_639_1 in_locale,
    bool in_width_limit_enabled,
    TextEnum::HorizontalLineAlignment in_horizontal,
    TextEnum::VerticalBlockAlignment in_vertical
    )
    : UIData(
        in_layout,
        in_base_colour,
        in_state_flag_tint_array_or_null
        )
    , _data(in_data)
    , _locale(in_locale)
    , _width_limit_enabled(in_width_limit_enabled)
    , _horizontal(in_horizontal)
    , _vertical(in_vertical)
{
    // Nop
}

UIDataString::~UIDataString()
{
    // Nop
}

void UIDataString::SetString(const std::string& in_data)
{
    if (_data == in_data)
    {
        return;
    }

    SetDirtyBit(UIDataDirty::TLayout, true);
    _data = in_data;

    return;
}

/// Make component type match what the data wants, default is UIComponentCanvas
void UIDataString::UpdateHierarchy(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    const int //in_index
    )
{
    bool dirty = false;
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentString* content = dynamic_cast<UIComponentString*>(in_out_component.get());
    auto font = in_param._text_manager->GetTextFont(in_param._default_text_style->_font_path);
    const TextLocale* const text_locale = in_param._text_manager->GetLocaleToken(_locale);

    if (nullptr == content)
    {
        auto text_block = std::make_unique<TextBlock>(
            *font,
            in_param._default_text_style->_font_size,
            in_param._default_text_style->_new_line_gap_ratio,
            _data,
            text_locale,
            VectorInt2(),
            _width_limit_enabled,
            0,
            _horizontal, 
            _vertical,
            in_param._default_text_style->_text_colour
            );

        auto new_content = std::make_unique<UIComponentString>(text_block);
        content = new_content.get();
        in_out_component = std::move(new_content);

        dirty = true;
    }
    else
    {
        if (true == content->Set(
            *font,
            in_param._default_text_style->_font_size,
            in_param._default_text_style->_new_line_gap_ratio,
            _width_limit_enabled,
            _horizontal, 
            _vertical,
            in_param._default_text_style->_text_colour
            ))
        {
            dirty = true;
        }
        if (true == content->SetText(
            _data,
            text_locale
            ))
        {
            dirty = true;
        }
    }

    if (true == dirty)
    {
        SetDirtyBit(UIDataDirty::TLayout, true);
    }

    SetDirtyBit(UIDataDirty::THierarchy, false);

    return;
}

void UIDataString::UpdateLayoutRender(
    IUIComponent& in_component,
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const UIScreenSpace& in_parent_screen_space
    )
{
    TSuper::UpdateLayoutRender(
        in_component,
        in_component_owner,
        in_param,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_parent_screen_space
        );

    UIComponentString* content = dynamic_cast<UIComponentString*>(&in_component);
    if (nullptr != content)
    {
        content->SetContainerSize(in_component_owner._node->GetTextureSize(
            in_param._draw_system
            ));
    }
    return;
}

const VectorInt2 UIDataString::GetContentSize(
    IUIComponent& in_component,
    const VectorInt2& in_target_size, 
    const float in_ui_scale,
    UIHierarchyNodeChildData& in_component_owner
    )
{
    return GetDesiredSize(
        in_component,
        in_target_size, 
        in_ui_scale,
        in_component_owner
        );
}

const VectorInt2 UIDataString::GetDesiredSize(
    IUIComponent& in_component,
    const VectorInt2& in_target_size, 
    const float in_ui_scale,
    UIHierarchyNodeChildData& //in_component_owner
    )
{
    UIComponentString* content = dynamic_cast<UIComponentString*>(&in_component);
    const VectorInt2 result = content ? content->GetDesiredSize(in_target_size, in_ui_scale) : VectorInt2();
    return result;
}
