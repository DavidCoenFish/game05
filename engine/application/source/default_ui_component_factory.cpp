#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float4.h"
#include "common/text/text_block.h"
#include "common/text/text_enum.h"
#include "common/text/text_font.h"
#include "common/text/text_manager.h"
#include "common/text/text_run.h"
#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_button.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_string.h"
#include "common/ui/ui_content/ui_content_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_text_run.h"

namespace
{
    enum class AlignmentHorizontal
    {
        TLeft,
        TMiddle,
        TRight,
    };
    enum class AlignmentVertical
    {
        TTop,
        TMiddle,
        TBottom,
    };

    constexpr int s_default_font_size = 16;
    constexpr int s_default_font_size_small = 12;
    constexpr float s_default_newling_ratio = 0.25f;
    constexpr int s_new_line_gap = 4; // DscMath::ScaleInt(s_default_font_size, s_default_newling_ratio);
    constexpr int s_new_line_gap_small = 3; // DscMath::ScaleInt(s_default_font_size, s_default_newling_ratio);
    constexpr float s_default_margin = s_default_font_size * 0.5f; // in pixels, or in em?
    constexpr float s_default_gap = s_default_font_size * 0.5f; // in pixels, or in em?
    constexpr float s_slow_fade = 1.0f;
    constexpr float s_quick_fade = 0.1f;

    typedef const std::filesystem::path& (*TGetPathRef)();
    const std::filesystem::path& GetFontPathDefault()
    {
        static std::filesystem::path s_data = std::filesystem::path("data") / "code2000.ttf";
        return s_data;
    }

    // Return copy or reference, want to avoid 
    typedef const UILayout& (*TGetUILayoutRef)();
    const UILayout& GetUILayout()
    {
        static UILayout s_layout = UILayout::FactoryFull();
        return s_layout;
    }
    const UILayout& GetUILayoutMargin()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutMarginBottomRightShrink()
    {
        // Todo: is y up the screen for ui, y==0 is bottom, y==1 is top screen
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 1.0f, -s_default_margin),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_margin),
            true,
            true
            );
        return s_layout;
    }
    const UILayout& GetUILayoutMarginMiddleShrinkVertical()
    {
        // Todo: is y up the screen for ui, y==0 is bottom, y==1 is top screen
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            false,
            true
            );
        return s_layout;
    }
    const UILayout& GetUILayoutQuadrant0()
    {
        // Todo: is y up or down the screen for ui
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutBannerLeft()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, 300.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.3333f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutRow()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 2.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutBottomRightShrink()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            true,
            true
            );
        return s_layout;
    }

    typedef const UICoord& (*TGetUICoordRef)();
    const UICoord& GetUICoordDefaultMargin()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin);
        return s_coord;
    }
    const UICoord& GetUICoordDefaultGap()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_gap);
        return s_coord;
    }

    typedef const UIBaseColour (*TGetUIBaseColour)();
    const UIBaseColour GetUIBaseColourDefault()
    {
        return UIBaseColour();
    }
    const UIBaseColour GetUIBaseColourStaggerClearTransparent()
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
            s_quick_fade,
            s_quick_fade
            );
    }
    const UIBaseColour GetUIBaseColourBlackSlowFade()
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
            true,
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
            s_slow_fade
            );
    }
    const UIBaseColour GetUIBaseColourRed()
    {
        return UIBaseColour(
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f),
            true
            );
    }

    typedef const VectorFloat4& (*TGetColour)();
    const VectorFloat4& GetColourTransparent()
    {
        static VectorFloat4 s_colour(0.0f, 0.0f, 0.0f, 0.0f);
        return s_colour;
    }
    const VectorFloat4& GetColourTransparentDark()
    {
        static VectorFloat4 s_colour(0.0f, 0.0f, 0.0f, 0.5f);
        return s_colour;
    }
    const VectorFloat4& GetColourRed()
    {
        static VectorFloat4 s_colour(1.0f, 0.0f, 0.0f, 1.0f);
        return s_colour;
    }
    const VectorFloat4& GetColourGreen()
    {
        static VectorFloat4 s_colour(0.0f, 1.0f, 0.0f, 1.0f);
        return s_colour;
    }
    const VectorFloat4& GetColourBlue()
    {
        static VectorFloat4 s_colour(0.0f, 0.0f, 1.0f, 1.0f);
        return s_colour;
    }
    const VectorFloat4& GetColourBlack()
    {
        static VectorFloat4 s_colour(0.0f, 0.0f, 0.0f, 1.0f);
        return s_colour;
    }
    const VectorFloat4& GetColourWhite()
    {
        static VectorFloat4 s_colour(1.0f, 1.0f, 1.0f, 1.0f);
        return s_colour;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault
        >
    const bool FactoryCanvas(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentCanvas* canvas = dynamic_cast<UIContentCanvas*>(in_out_content.get());
        if (nullptr == canvas)
        {
            in_out_content = std::make_unique<UIContentCanvas>(
                in_get_base_colour(),
                in_get_layout_ref());
            return true;
        }
        else
        {
            if (true == canvas->SetBase(
                in_get_base_colour(),
                in_get_layout_ref()))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault
        >
    const bool FactoryButton(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentButton* content = dynamic_cast<UIContentButton*>(in_out_content.get());
        if (nullptr == content)
        {
            in_out_content = std::make_unique<UIContentButton>(
                in_get_base_colour(),
                in_get_layout_ref());
            return true;
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(),
                in_get_layout_ref()))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetPathRef in_get_path_ref = GetFontPathDefault,
        int in_font_size = s_default_font_size,
        int in_new_line_gap = s_new_line_gap,
        TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Top,
        bool in_width_limit = false,
        TGetColour in_get_text_colour_ref = GetColourBlack
        >
    const bool FactoryString(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
        auto font = in_param._text_manager->GetTextFont(in_get_path_ref());
        const float new_line_gap_ratio = static_cast<float>(in_new_line_gap) / static_cast<float>(in_font_size);

        UIContentString* content = dynamic_cast<UIContentString*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            // Make something with the data we have on hand
            auto text_block = std::make_unique<TextBlock>(
                *font,
                in_font_size,
                new_line_gap_ratio,
                "",
                nullptr,
                VectorInt2(),
                in_width_limit,
                0,
                in_horizontal,
                in_vertical,
                in_get_text_colour_ref()
                );
            in_out_content = std::move(std::make_unique<UIContentString>(
                in_get_base_colour(),
                in_get_layout_ref(), 
                text_block
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(),
                    in_get_layout_ref()))
            {
                dirty = true;
            }

            if (true == content->Set(
                *font, 
                in_font_size, 
                new_line_gap_ratio,
                in_width_limit, 
                in_horizontal, 
                in_vertical,
                in_get_text_colour_ref()
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        int in_em_size = s_default_font_size,
        TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Top,
        bool in_width_limit_enabled = false
        >
    const bool FactoryTextRun(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentTextRun* content = dynamic_cast<UIContentTextRun*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            // Make something with the data we have on hand
            auto text_run = std::make_unique<TextRun>(
                std::vector<std::shared_ptr<ITextRunData>>(),
                VectorInt2(),
                in_width_limit_enabled,
                0,
                in_horizontal,
                in_vertical,
                in_em_size
                );
            in_out_content = std::move(std::make_unique<UIContentTextRun>(
                in_get_base_colour(),
                in_get_layout_ref(), 
                text_run
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(),
                    in_get_layout_ref()))
            {
                dirty = true;
            }

            if (true == content->Set(
                in_width_limit_enabled,
                in_horizontal,
                in_vertical,
                in_em_size
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        StackOrientation in_orientation = StackOrientation::TVertical,
        TGetUICoordRef in_get_gap_ref = GetUICoordDefaultGap
        >
    const bool FactoryStack(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& //in_param
        )
    {
        UIContentStack* content = dynamic_cast<UIContentStack*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIContentStack>(
                in_get_base_colour(),
                in_get_layout_ref(),
                in_orientation,
                in_get_gap_ref()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(),
                in_get_layout_ref(),
                in_orientation,
                in_get_gap_ref()
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }
}

void DefaultUIComponentFactory::Populate(
    UIManager& in_ui_manager
    )
{
    in_ui_manager.AddContentFactory("UIDataString", FactoryString<>);
    // Button text
    in_ui_manager.AddContentFactory("string_middle_em", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_small_right", FactoryString<
        GetUILayoutBottomRightShrink,
        GetUIBaseColourDefault,
        GetFontPathDefault,
        s_default_font_size_small,
        s_new_line_gap_small,
        TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment::Top
        >);

    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<>);
    in_ui_manager.AddContentFactory("text_run_small_right", FactoryTextRun<
        GetUILayoutBottomRightShrink,
        GetUIBaseColourDefault,
        s_default_font_size_small,
        TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment::Top
        >);

    in_ui_manager.AddContentFactory("UIDataTextRunWrap", FactoryTextRun<
        GetUILayoutQuadrant0,
        GetUIBaseColourDefault,
        s_default_font_size,
        TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment::Top,
        true
        >);

    in_ui_manager.AddContentFactory("UIDataButton", FactoryButton<
        GetUILayoutRow, 
        GetUIBaseColourRed
        >);

    in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<>);
    in_ui_manager.AddContentFactory("canvas_debug_quad0", FactoryCanvas<
        GetUILayoutQuadrant0, 
        GetUIBaseColourRed
        >);
    in_ui_manager.AddContentFactory("canvas_banner_left", FactoryCanvas<
        GetUILayoutBannerLeft, 
        GetUIBaseColourStaggerClearTransparent
        >);

    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutMarginBottomRightShrink,
        GetUIBaseColourStaggerClearTransparent,
        StackOrientation::TVertical,
        GetUICoordDefaultGap
        >);

    in_ui_manager.AddContentFactory("stack_vertical_middle", FactoryStack<
        GetUILayoutMarginMiddleShrinkVertical,
        GetUIBaseColourStaggerClearTransparent,
        StackOrientation::TVertical,
        GetUICoordDefaultGap
        >);
    return;
}

const UIDataTextRunStyle* const DefaultUIComponentFactory::GetDefaultTextStyle()
{
    static UIDataTextRunStyle s_data(
        GetFontPathDefault(),
        s_default_font_size,
        s_default_newling_ratio,
        GetColourBlack(),
        GetColourWhite()
        );
    return &s_data;
}

const std::string DefaultUIComponentFactory::GetTextRunTagSmallFont()
{
    return std::string("<Size ") + std::to_string(s_default_font_size_small) + ">";
}

