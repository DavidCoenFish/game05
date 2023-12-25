#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_string.h"

#include "common/ui/ui_component/ui_component_string.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/text/text_manager.h"
#include "common/text/text_block.h"

UIDataString::UIDataString(
#ifdef _DEBUG
    const std::string& in_debug_name,
#endif
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const UITintColour& in_tint_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    const std::string& in_data,
    const LocaleISO_639_1 in_locale,
    const bool in_width_limit_enabled,
    const TextEnum::HorizontalLineAlignment in_horizontal,
    const TextEnum::VerticalBlockAlignment in_vertical
    )
    : UIData(
#ifdef _DEBUG
        in_debug_name,
#endif
        in_layout,
        in_base_colour,
        in_tint_colour,
        in_array_effect_data
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

    SetDirtyBit(UIDataDirty::TComponentDirty, true);
    _data = in_data;

    return;
}

const bool UIDataString::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam& in_param
    )
{
    bool dirty = false;

    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentString* content = dynamic_cast<UIComponentString*>(in_out_component.get());
    auto font = in_param._text_manager->GetTextFont(in_param._default_text_style->_font_path);
    const TextLocale* const text_locale = in_param._text_manager->GetLocaleToken(_locale);

    // the tint colour is normally used to influence rendering children shaders
    //const VectorFloat4 colour = GetTintColour().GetTintColour(0.0f);

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
            // or should we use VectorFloat4::s_white and use the UIBaseColour::_tint to multiply with the white?
            in_param._default_text_style->_text_colour
            //VectorFloat4::s_white
            //colour
            );

        auto new_content = std::make_unique<UIComponentString>(
            text_block
            );
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
            //VectorFloat4::s_white
            ))
        {
            dirty = true;
        }

        // potential problem, string component change could change layout if layout is shrink...? could workaround by dirtying layout on text change
        // another potenial fix is to have better fedelity, or the component choose to dirty layout on content change?
        // general solution, if layout is shrink, then component change invalidates layout
        if (true == content->SetText(
            _data,
            text_locale
            ))
        {
            dirty = true;
        }
    }

    //SetDirtyBit(UIDataDirty::THierarchy, false);

    return dirty;
}
