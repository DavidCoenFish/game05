#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float4.h"
#include "common/text/text_manager.h"
#include "common/text/text_enum.h"
#include "common/text/text_font.h"
#include "common/text/text_block.h"
#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_string.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_data/ui_data_string.h"

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
    constexpr int s_default_em_size = 16;
    constexpr float s_default_margin = s_default_em_size * 0.5f; // in pixels, or in em?

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
    const UILayout& GetUILayoutFullScreenMarginBottomRight()
    {
        // Todo: is y up or down the screen for ui
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, -1.0f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, -1.0f)
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

    const UICoord& GetUICoordDefaultX()
    {
        static UICoord s_coord(UICoord::ParentSource::X, 0.0f, s_default_margin);
        return s_coord;
    }
    const UICoord& GetUICoordDefaultY()
    {
        static UICoord s_coord(UICoord::ParentSource::Y, 0.0f, s_default_margin);
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

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        bool in_clear_background = false,
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
        int in_font_size = 16,
        int in_new_line_gap = 4,
        TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::MiddleEM,
        bool in_clear_background = false,
        TGetColour in_get_clear_colour_ref = GetColourTransparent
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
                false,
                0,
                in_horizontal,
                in_vertical
                );
            in_out_content = std::move(std::make_unique<UIContentString>(
                in_clear_background,
                in_get_clear_colour_ref(),
                in_get_layout_ref(), 
                text_block,
                in_font_size,
                new_line_gap_ratio
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
                in_horizontal, 
                in_vertical
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        int in_font_size = 16,
        bool in_clear_background = false,
        TGetColour in_get_clear_colour_ref = GetColourTransparent
        >
    const bool FactoryTextRun(
        std::unique_ptr<IUIContent>&, //in_out_content,
        const UIContentFactoryParam& //in_param
        )
    {
        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayoutFullScreen,
        StackOrientation in_orientation = StackOrientation::TVertical,
        TGetUICoordRef in_get_gap_ref = GetUICoordDefaultY,
        bool in_shrink_to_fit = false,
        bool in_clear_background = false,
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
                in_get_gap_ref(),
                in_shrink_to_fit
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
                in_get_gap_ref(),
                in_shrink_to_fit
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
    //in_ui_manager.AddContentFactory("UIDataString", FactoryString<>);
    in_ui_manager.AddContentFactory("UIDataString", FactoryString<
        GetUILayoutQuadrant0,
        GetFontPathDefault,
        64,
        16,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::MiddleEM,
        true,
        GetColourGreen
        >);
    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<>);
    //in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<>);
    in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<GetUILayoutFullScreenMargin, true, GetColourRed>);
    //in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<GetUILayoutQuadrant0, true, GetColourRed>);


    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutFullScreenMarginBottomRight,
        StackOrientation::TVertical,
        GetUICoordDefaultY,
        true,
        true,
        GetColourRed
        >);

}
