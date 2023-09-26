#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/math.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float4.h"
#include "common/text/text_block.h"
#include "common/text/text_enum.h"
#include "common/text/text_font.h"
#include "common/text/text_manager.h"
#include "common/text/text_run.h"
#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_string.h"
#include "common/ui/ui_content/ui_content_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
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
    constexpr float s_default_newling_ratio = 0.25f;
    constexpr int s_new_line_gap = 4; // Math::ScaleInt(s_default_font_size, s_default_newling_ratio);
    constexpr float s_default_margin = s_default_font_size * 0.5f; // in pixels, or in em?
    constexpr float s_default_gap = s_default_font_size * 0.25f; // in pixels, or in em?

    typedef const std::filesystem::path& (*TGetPathRef)();

    const std::filesystem::path& GetFontPathDefault()
    {
        static std::filesystem::path s_data = std::filesystem::path("data") / "code2000.ttf";
        return s_data;
    }

    // Return copy or reference, want to avoid 
    typedef const UILayout& (*TGetUILayoutRef)();

    const UILayout& GetUILayoutFullScreen()
    {
        static UILayout s_layout = UILayout::FactoryFull();
        return s_layout;
    }
    const UILayout& GetUILayoutFullScreenMargin()
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
    const UILayout& GetUILayoutFullScreenMarginBottomRightShrink()
    {
        // Todo: is y up the screen for ui, y==0 is bottom, y==1 is top screen
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 1.0f, -s_default_margin),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_margin),
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

    typedef const VectorFloat4& (*TGetColour)();
    const VectorFloat4& GetColourTransparent()
    {
        static VectorFloat4 s_colour(0.0f, 0.0f, 0.0f, 0.0f);
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
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        bool in_clear_background = true,
        TGetColour in_get_clear_colour_ref = GetColourTransparent
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
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref());
            return true;
        }
        else
        {
            if (true == canvas->SetBase(
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref()))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        TGetPathRef in_get_path_ref = GetFontPathDefault,
        int in_font_size = s_default_font_size,
        int in_new_line_gap = s_new_line_gap,
        TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Top,
        bool in_clear_background = true,
        TGetColour in_get_clear_colour_ref = GetColourTransparent,
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
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref(), 
                text_block
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_clear_background,
                    in_get_clear_colour_ref(),
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
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        int in_em_size = s_default_font_size,
        TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Top,
        bool in_clear_background = true,
        TGetColour in_get_clear_colour_ref = GetColourTransparent,
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
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref(), 
                text_run
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_clear_background,
                    in_get_clear_colour_ref(),
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
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        StackOrientation in_orientation = StackOrientation::TVertical,
        TGetUICoordRef in_get_gap_ref = GetUICoordDefaultGap,
        bool in_clear_background = true,
        TGetColour in_get_clear_colour_ref = GetColourTransparent
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
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref(),
                in_orientation,
                in_get_gap_ref()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->Set(
                in_clear_background,
                in_get_clear_colour_ref(),
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
    in_ui_manager.AddContentFactory("UIDataStringRight", FactoryString<
        GetUILayoutFullScreen,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::TopEM
        >);
    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<>);
    in_ui_manager.AddContentFactory("UIDataTextRunRight", FactoryTextRun<
        GetUILayoutFullScreen,
        s_default_font_size,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::TopEM
        >);

    in_ui_manager.AddContentFactory("UIDataTextRunWrap", FactoryTextRun<
        GetUILayoutQuadrant0,
        s_default_font_size,
        TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment::Top,
        true,
        GetColourTransparent,
        true
        >);
    in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<>);
    in_ui_manager.AddContentFactory("UIDataContainerDebug", FactoryCanvas<GetUILayoutFullScreenMargin, true, GetColourRed>);


    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutFullScreenMarginBottomRightShrink,
        StackOrientation::TVertical,
        GetUICoordDefaultGap,
        true,
        GetColourTransparent
        >);

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
