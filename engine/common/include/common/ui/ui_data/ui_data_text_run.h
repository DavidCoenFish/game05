#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/locale/locale_enum.h"
#include "common/text/text_enum.h"
#include "common/math/vector_float4.h"

class ITextRunData;
class LocaleSystem;
class TextManager;
//enum class LocaleISO_639_1;
typedef std::size_t THashKey;

// Gather defaults
struct UIDataTextRunStyle
{
    explicit UIDataTextRunStyle(
        const std::filesystem::path& in_font_path = "",
        const int in_font_size = 0,
        const float in_new_line_gap_ratio = 0.0f,
        const VectorFloat4& in_text_colour = VectorFloat4(),
        const VectorFloat4& in_icon_colour = VectorFloat4()
        );

    std::filesystem::path _font_path;
    int _font_size;
    float _new_line_gap_ratio;
    VectorFloat4 _text_colour;
    VectorFloat4 _icon_colour;
};

class UIDataTextRun : public UIData
{
public:
    /// Use < and > as markup flags, and << as escapes (don't need >> as escape if set to auto consume > at end of a mate block)
    /// Don't have xml open/ close, just a stream of markup
    /// """<Locale [locale_key]><Icon [id]><Colour 1.0 0.5 0.0 1.0>red<DefaultColour> raw text <Font path_to_font>other font text <DefaultFont>back to default font"""
    static void BuildTextRunData(
        std::vector<std::shared_ptr<ITextRunData>>& out_run_data,
        const std::string& in_markup_string_utf8,
        const LocaleISO_639_1 in_locale,
        TextManager& in_text_manager,
        LocaleSystem& in_locale_system,
        const UIDataTextRunStyle& in_default_style
        );

    UIDataTextRun(
#ifdef _DEBUG
        const std::string& in_debug_name,
#endif
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const UITintColour& in_tint_colour,
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,

        const std::string& in_markupStringUtf8,
        const LocaleISO_639_1 in_locale = LocaleISO_639_1::Default,
        const bool in_width_limit_enabled = false,
        const TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
        const TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Bottom
        );
    virtual ~UIDataTextRun();

    //const THashKey GetHashKey() const;

    void SetMarkupStringUtf8(const std::string& in_markup_string_utf8);
    void SetLocale(const LocaleISO_639_1 in_locale);

    // Allow external to just pass in an array of text run data
    void SetData(const std::vector<std::shared_ptr<ITextRunData>>& in_data);

private:
    void BuildData(
        TextManager& in_text_manager,
        LocaleSystem& in_locale_system,
        const UIDataTextRunStyle& in_default_style
        );

    virtual const bool ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param
        ) override;

private:
    std::string _markup_string_utf8;
    LocaleISO_639_1 _locale;
    //int _change_id;

    bool _dirty;
    std::vector<std::shared_ptr<ITextRunData>> _data;

    bool _width_limit_enabled;
    TextEnum::HorizontalLineAlignment _horizontal;
    TextEnum::VerticalBlockAlignment _vertical;

};

