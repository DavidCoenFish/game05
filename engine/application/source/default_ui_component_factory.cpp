#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/vector_int4.h"
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

    const UILayout& GetUILayoutDefault()
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

    template<
        TGetUILayoutRef GetLayoutRef = GetUILayoutDefault
        >
    const bool FactoryCanvas(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentCanvas* canvas = dynamic_cast<UIContentCanvas*>(in_out_content.get());
        if (nullptr == canvas)
        {
            in_out_content = std::make_unique<UIContentCanvas>(GetLayoutRef());
            return true;
        }
        else
        {
            if (true == canvas->SetLayout(GetLayoutRef()))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef GetLayout = GetUILayoutDefault,
        TGetPathRef GetPath = GetFontPathDefault,
        int FontSize = 16,
        int NewLineHeight = 16,
        TextEnum::HorizontalLineAlignment Horizontal = TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment Vertical = TextEnum::VerticalBlockAlignment::MiddleEM,
        int EMSize = 16
        >
    const bool FactoryString(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
        auto font = in_param._text_manager->GetTextFont(GetPath());

        UIContentString* content = dynamic_cast<UIContentString*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            // Make something with the data we have on hand
            auto text_block = std::make_unique<TextBlock>(
                *font,
                FontSize,
                NewLineHeight,
                "",
                nullptr,
                VectorInt2(),
                false,
                0,
                Horizontal,
                Vertical
                );
            in_out_content = std::move(std::make_unique<UIContentString>(GetDefaultUILayout(), text_block));
            dirty = true;
        }
        else
        {
            if (true == content->SetLayout(GetDefaultUILayout()))
            {
                dirty = true;
            }

            if (true == content->SetFont(*font, FontSize, NewLineHeight))
            {
                dirty = true;
            }

            if (true == content->SetAlignment(Horizontal, Vertical, EMSize))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUILayoutRef GetLayout = GetUILayoutDefault,
        int FontSize = 16
        >
    const bool FactoryTextRun(
        std::unique_ptr<IUIContent>&, //in_out_content,
        const UIContentFactoryParam& //in_param
        )
    {
        return false;
    }

    template<
        TGetUILayoutRef GetLayout = GetUILayoutDefault,
        StackOrientation Orientation = StackOrientation::TVertical,
        TGetUICoordRef GetGap = GetUICoordDefaultY,
        bool ShrinkToFit = false
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
            auto new_content = std::make_unique<UIContentStack>(
                GetDefaultUILayout(),
                Orientation,
                GetGap(),
                ShrinkToFit
                );
            in_out_content = std::move(std::make_unique<UIContentString>(new_content));
        }
        else
        {
            content->Set(
                GetDefaultUILayout(),
                Orientation,
                GetGap(),
                ShrinkToFit
                );
        }

        return false;
    }
}

void DefaultUIComponentFactory::Populate(
    UIManager& in_ui_manager
    )
{
    in_ui_manager.AddContentFactory("UIDataString", FactoryString<GetUILayoutDefault>);
    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<GetUILayoutDefault>);
    in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas<GetUILayoutDefault>);
    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutFullScreenMarginBottomRight,
        StackOrientation::TVertical,
        GetUICoordDefaultY
        >);

}
