#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_text_run.h"

#include "common/locale/locale_system.h"
#include "common/text/text_font.h"
#include "common/text/text_manager.h"
#include "common/text/text_run_data_icon.h"
#include "common/text/text_run_data_string.h"
#include "common/ui/ui_component/ui_component_text_run.h"
#include "common/ui/ui_hierarchy_node.h"

namespace
{
    class Tokenizer;
    struct FactoryParam;

    typedef std::function<std::shared_ptr<ITextRunData>(FactoryParam& in_param)> TFactory;
    typedef std::map<std::string, TFactory> TMapFactory;
    
    enum class DSCTokenType
    {
        TValue,
        TMarkup,
        TMarkupParam,
        TMarkupEnd,
        TError
    };

    class Tokenizer
    {
    public:
        Tokenizer(const std::string& in_string)
            : _string(in_string)
            , _cursor(0)
            , _inside_markup(false)
        {
            // Nop
        }

        // return true is another token is found, 
        const bool ConsumeToken(
            std::string& out_token,
            DSCTokenType& out_token_type
            )
        {
            out_token.clear();
            out_token_type = DSCTokenType::TError;

            if (_string.length() <= _cursor)
            {
                return false;
            }

            bool this_token_is_markup = false;
            while (_cursor < _string.length())
            {
                const char value = _string[_cursor];

                if (true == _inside_markup)
                {
                    if ('>' == value)
                    {
                        _cursor += 1;
                        _inside_markup = false;
                        if (true == out_token.empty())
                        {
                            out_token_type = DSCTokenType::TMarkupEnd;
                        }
                        else
                        {
                            out_token_type = DSCTokenType::TMarkupParam;
                        }
                        return true;
                    }
                    else if (' ' == value)
                    {
                        _cursor += 1;
                        if (false == out_token.empty())
                        {
                            if (true == this_token_is_markup)
                            {
                                out_token_type = DSCTokenType::TMarkup;
                            }
                            else
                            {
                                out_token_type = DSCTokenType::TMarkupParam;
                            }
                            return true;
                        }
                        // else, skip over whitespace
                    }
                    else
                    {
                        _cursor += 1;
                        out_token += value;
                    }
                }
                else
                {
                    if ('<' == value)
                    {
                        if ((_cursor + 1 < _string.length()) &&
                            ('<' == _string[_cursor + 1]))
                        {
                            _cursor += 2;
                            out_token += value;
                        }
                        else if (false == out_token.empty())
                        {
                            out_token_type = DSCTokenType::TValue;
                            return true;
                        }
                        else
                        {
                            _cursor += 1;
                            this_token_is_markup = true;
                            _inside_markup = true;
                        }
                    }
                    else
                    {
                        out_token += value;
                        _cursor += 1;
                    }
                }
            }

            if (false == out_token.empty())
            {
                if (false == _inside_markup)
                {
                    out_token_type = DSCTokenType::TValue;
                }
            }

            return true;
        }

    private:
        const std::string& _string;
        int _cursor;
        bool _inside_markup;
    };

    struct FactoryParam
    {
        TextManager& _text_manager;
        LocaleSystem& _locale_system;
        const UIDataTextRunStyle& _default_style;
        UIDataTextRunStyle _current_style;
        TextFont* _current_font;
        Tokenizer& _tokenizer;
        const LocaleISO_639_1 _locale;
    };

    const TMapFactory& GetMapFactory()
    {
        static TMapFactory s_map_factory;

        if (0 == s_map_factory.size())
        {
            // Locale
            s_map_factory["Locale"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                if (true == in_param._tokenizer.ConsumeToken(
                    token,
                    token_type
                    ))
                {
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }

                    LocaleISO_639_1 locale = in_param._locale;
                    std::string string_utf8 = in_param._locale_system.GetValue(locale, token);
                    if (true == string_utf8.empty())
                    {
                        locale = LocaleISO_639_1::Default;
                        string_utf8 = token;
                    }
                    return std::make_shared<TextRunDataString>(
                        string_utf8,
                        in_param._text_manager.GetLocaleToken(locale),
                        in_param._current_font,
                        in_param._current_style._font_size,
                        in_param._current_style._new_line_gap_ratio,
                        in_param._current_style._text_colour
                        );
                }

                return nullptr;
            });

            // Icon
            s_map_factory["Icon"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                if (true == in_param._tokenizer.ConsumeToken(
                    token,
                    token_type
                    ))
                {
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }

                    const int icon_id = std::stoi(token);

                    return std::make_shared<TextRunDataIcon>(
                        icon_id,
                        in_param._text_manager.GetIconFont(),
                        in_param._current_style._icon_colour,
                        in_param._current_style._new_line_gap_ratio
                        );
                }

                return nullptr;
            });

            // Colour
            s_map_factory["Colour"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                for (int index = 0; index < 4; ++index)
                {
                    if (false == in_param._tokenizer.ConsumeToken(
                        token,
                        token_type
                        ))
                    {
                        return nullptr;
                    }
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }
                    in_param._current_style._text_colour[index] = std::stof(token);
                }
                return nullptr;
            });

            // DefaultColour
            s_map_factory["DefaultColour"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                in_param._current_style._text_colour = in_param._default_style._text_colour;
                return nullptr;
            });

            // IconTint
            s_map_factory["IconTint"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                for (int index = 0; index < 4; ++index)
                {
                    if (false == in_param._tokenizer.ConsumeToken(
                        token,
                        token_type
                        ))
                    {
                        return nullptr;
                    }
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }
                    in_param._current_style._icon_colour[index] = std::stof(token);
                }
                return nullptr;
            });

            // DefaultIconTint
            s_map_factory["DefaultIconTint"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                in_param._current_style._icon_colour = in_param._default_style._icon_colour;
                return nullptr;
            });

            // Font
            s_map_factory["Font"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                if (true == in_param._tokenizer.ConsumeToken(
                    token,
                    token_type
                    ))
                {
                    if (token_type == DSCTokenType::TMarkupParam)
                    {
                        in_param._current_style._font_path = token;
                        in_param._current_font = in_param._text_manager.GetTextFont(in_param._current_style._font_path);
                    }
                }

                return nullptr;
            });

            // DefaultFont
            s_map_factory["DefaultFont"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                in_param._current_style._font_path = in_param._default_style._font_path;
                in_param._current_font = in_param._text_manager.GetTextFont(in_param._current_style._font_path);
                return nullptr;
            });

            // Size 
            s_map_factory["Size"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                if (true == in_param._tokenizer.ConsumeToken(
                    token,
                    token_type
                    ))
                {
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }

                    in_param._current_style._font_size = std::stoi(token);
                }

                return nullptr;
            });

            // DefaultSize 
            s_map_factory["DefaultSize"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                in_param._current_style._font_size = in_param._default_style._font_size;
                return nullptr;
            });

            // GapRatio
            s_map_factory["GapRatio"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                std::string token;
                DSCTokenType token_type = DSCTokenType::TError;
                if (true == in_param._tokenizer.ConsumeToken(
                    token,
                    token_type
                    ))
                {
                    if (token_type != DSCTokenType::TMarkupParam)
                    {
                        return nullptr;
                    }

                    in_param._current_style._new_line_gap_ratio = std::stof(token);
                }

                return nullptr;
            });

            // DefaultGapRatio
            s_map_factory["DefaultGapRatio"] = TFactory([](FactoryParam& in_param)->std::shared_ptr<ITextRunData>{
                in_param._current_style._new_line_gap_ratio = in_param._default_style._new_line_gap_ratio;
                return nullptr;
            });

        }

        return s_map_factory;
    }

    std::shared_ptr<ITextRunData> TextFactory(const std::string& in_string_utf8, FactoryParam& in_param)
    {
        return std::make_shared<TextRunDataString>(
            in_string_utf8,
            //in_param._text_manager.GetLocaleToken(in_param._locale),
            in_param._text_manager.GetLocaleToken(LocaleISO_639_1::Default),
            in_param._current_font,
            in_param._current_style._font_size,
            in_param._current_style._new_line_gap_ratio,
            in_param._current_style._text_colour
            );
    }

}

UIDataTextRunStyle::UIDataTextRunStyle(
    const std::filesystem::path& in_font_path,
    const int in_font_size,
    const float in_new_line_gap_ratio,
    const VectorFloat4& in_text_colour,
    const VectorFloat4& in_icon_colour
    )
    : _font_path(in_font_path)
    , _font_size(in_font_size)
    , _new_line_gap_ratio(in_new_line_gap_ratio)
    , _text_colour(in_text_colour)
    , _icon_colour(in_icon_colour)
{
    // Nop
}

void UIDataTextRun::BuildTextRunData(
    std::vector<std::shared_ptr<ITextRunData>>& out_run_data,
    const std::string& in_markup_string_utf8,
    const LocaleISO_639_1 in_locale,
    TextManager& in_text_manager,
    LocaleSystem& in_locale_system,
    const UIDataTextRunStyle& in_default_style
    )
{
    Tokenizer tokenizer(in_markup_string_utf8);
    out_run_data.clear();

    FactoryParam param({
        in_text_manager,
        in_locale_system,
        in_default_style,
        in_default_style,
        in_text_manager.GetTextFont(in_default_style._font_path),
        tokenizer,
        in_locale
        });

    std::string token;
    DSCTokenType token_type = DSCTokenType::TError;
    while (tokenizer.ConsumeToken(token, token_type))
    {
        switch (token_type)
        {
        default:
            break;
        case DSCTokenType::TValue:
            {
                auto run = TextFactory(token, param);
                if (nullptr != run)
                {
                    out_run_data.push_back(run);
                }
            }
            break;
        case DSCTokenType::TMarkup:
            {
                const auto& map_factory = GetMapFactory();
                auto found = map_factory.find(token);
                if (found != map_factory.end())
                {
                    auto run = found->second(param);
                    if (nullptr != run)
                    {
                        out_run_data.push_back(run);
                    }
                }
            }
            break;

        }

    }
}

UIDataTextRun::UIDataTextRun(
#ifdef _DEBUG
    const std::string& in_debug_name,
#endif
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const UITintColour& in_tint_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,

    const std::string& in_markupStringUtf8,
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
    , _markup_string_utf8(in_markupStringUtf8)
    , _locale(in_locale)
    , _dirty(true)
    , _data()
    , _width_limit_enabled(in_width_limit_enabled)
    , _horizontal(in_horizontal)
    , _vertical(in_vertical)
{
    // Nop
}

UIDataTextRun::~UIDataTextRun()
{
    // Nop
}

void UIDataTextRun::SetMarkupStringUtf8(const std::string& in_markup_string_utf8)
{
    if (_markup_string_utf8 != in_markup_string_utf8)
    {
        _markup_string_utf8 = in_markup_string_utf8;
        _dirty = true;
        SetDirtyBit(UIDataDirty::TComponentDirty, true);
    }
    return;
}

void UIDataTextRun::SetLocale(const LocaleISO_639_1 in_locale)
{
    if (_locale != in_locale)
    {
        _locale = in_locale;
        _dirty = true;
        SetDirtyBit(UIDataDirty::TComponentDirty, true);
    }
    return;
}

// Allow external to just pass in an array of text run data
void UIDataTextRun::SetData(const std::vector<std::shared_ptr<ITextRunData>>& in_data)
{
    SetDirtyBit(UIDataDirty::TComponentDirty, true);
    _data = in_data;
    _dirty = false;
    return;
}

void UIDataTextRun::BuildData(
    TextManager& in_text_manager,
    LocaleSystem& in_locale_system,
    const UIDataTextRunStyle& in_default_style
    )
{
    if (true == _dirty)
    {
        _dirty = false;
        _data.clear();
        BuildTextRunData(
            _data,
            _markup_string_utf8,
            _locale,
            in_text_manager,
            in_locale_system,
            in_default_style
            );
    }

    return;
}


const bool UIDataTextRun::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam& in_param
    )
{
    bool dirty = false;

    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentTextRun* component = dynamic_cast<UIComponentTextRun*>(in_out_component.get());
    //auto font = in_param._text_manager->GetTextFont(in_param._default_text_style->_font_path);
    //const TextLocale* const text_locale = in_param._text_manager->GetLocaleToken(_locale);

    BuildData(
        *in_param._text_manager,
        *in_param._locale_system,
        *in_param._default_text_style
        );

    // the tint colour is normally used to influence rendering children shaders
    //const VectorFloat4 colour = GetTintColour().GetTintColour(0.0f);

    if (nullptr == component)
    {
        auto text_run = std::make_unique<TextRun>(
            _data,
            VectorInt2::s_zero,
            _width_limit_enabled,
            0,
            _horizontal,
            _vertical,
            in_param._default_text_style->_font_size,
            in_param._ui_scale
            );

        auto new_component = std::make_unique<UIComponentTextRun>(
            text_run
            );
        in_out_component = std::move(new_component);

        dirty = true;
    }
    else
    {
        if (true == component->Set(
            _data,
            _width_limit_enabled,
            _horizontal, 
            _vertical,
            in_param._default_text_style->_font_size,
            in_param._ui_scale
            ))
        {
            dirty = true;
        }
    }

    //SetDirtyBit(UIDataDirty::THierarchy, false);

    return dirty;
}

