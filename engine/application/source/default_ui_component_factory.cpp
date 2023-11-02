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
#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_button.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_component/ui_component_disable.h"
#include "common/ui/ui_component/ui_component_effect.h"
#include "common/ui/ui_component/ui_component_grid.h"
#include "common/ui/ui_component/ui_component_stack.h"
#include "common/ui/ui_component/ui_component_string.h"
#include "common/ui/ui_component/ui_component_slider.h"
#include "common/ui/ui_component/ui_component_text_run.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_data/ui_data_disable.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_float.h"
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
    constexpr float s_default_button_width = 144.0f; // https://uxmovement.com/mobile/optimal-size-and-spacing-for-mobile-buttons/
    constexpr float s_slow_fade = 1.0f;
    constexpr float s_quick_fade = 0.1f;

    typedef const std::filesystem::path& (*TGetPathRef)();
    const std::filesystem::path& GetFontPathDefault()
    {
        static std::filesystem::path s_data = std::filesystem::path("data") / "code2000.ttf";
        return s_data;
    }
    const std::filesystem::path& GetFontPathFixedWidth()
    {
        static std::filesystem::path s_data = std::filesystem::path("data") / "mono_spatial.ttf";
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
    const UILayout& GetUILayoutMarginTiny()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-0.25f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-0.25f)),
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
    const UILayout& GetUILayoutMarginTop()
    {
        // Todo: is y up the screen for ui, y==0 is bottom, y==1 is top screen
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            false,
            true
            );
        return s_layout;
    }

    const UILayout& GetUILayoutQuadrant0() //lover left
    {
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
    const UILayout& GetUILayoutQuadrant1() //upper left
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f)
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
    const UILayout& GetUILayoutModal()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.75f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutShrink()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            true,
            true
            );
        return s_layout;
    }
    const UILayout& GetUILayoutShrinkVertical()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            false,
            true
            );
        return s_layout;
    }

    const UILayout& GetUILayoutCheckboxLeft()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutWidget()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutSliderHorizontal()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    typedef const UICoord& (*TGetUICoordRef)();
    const UICoord& GetUICoordNone()
    {
        static UICoord s_coord;
        return s_coord;
    }
    const UICoord& GetUICoordDefaultMargin()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin);
        return s_coord;
    }
    const UICoord& GetUICoordDefaultGap()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin);
        return s_coord;
    }
    const UICoord& GetUICoordDefaultGapHalf()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin * 0.5f);
        return s_coord;
    }
    const UICoord& GetUICoordDefaultGapQuater()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin * 0.25f);
        return s_coord;
    }

    typedef const std::shared_ptr<const UIComponentEffect::TStateFlagTintArray>& (*TGetUIStateFlagTintArray)();
    const std::shared_ptr<const UIComponentEffect::TStateFlagTintArray>& GetUIStateFlagTintArray()
    {
        static const std::shared_ptr<const UIComponentEffect::TStateFlagTintArray> s_data;
        return s_data;
    }
    const std::shared_ptr<const UIComponentEffect::TStateFlagTintArray>& GetUIStateFlagTintArrayDefault()
    {
        static const UIComponentEffect::TStateFlagTintArray s_data_array({
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f), // TNone
            VectorFloat4(0.2f, 0.2f, 1.0f, 1.0f), // THover
            VectorFloat4(0.0f, 0.25f, 0.75f, 1.0f), // TTouch
            VectorFloat4(0.2f, 0.45f, 0.75f, 1.0f), // TTouch THover
            VectorFloat4(0.25f, 0.0f, 1.0f, 1.0f), // TSelected
            VectorFloat4(0.45f, 0.2f, 1.0f, 1.0f), // TSelected THover
            VectorFloat4(0.25f, 0.25f, 0.75f, 1.0f), // TSelected TTouch
            VectorFloat4(0.45f, 0.45f, 0.75f, 1.0f)  // TSelected TTouch THover
            });
        static const std::shared_ptr<const UIComponentEffect::TStateFlagTintArray> s_data = std::make_shared<const UIComponentEffect::TStateFlagTintArray>(s_data_array);
        return s_data;
    }

    typedef const std::vector<UIComponentGridSizeData>& (*TGetUIGridSizeData)();
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataDefaultHorizontal()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 1.0f))
        });
        return s_data;
    }
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataDefaultVertical()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 1.0f))
        });
        return s_data;
    }
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataBigRowVertical()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.5f))
        });
        return s_data;
    }

    const std::vector<UIComponentGridSizeData>& GetUIGridSliderHorizontalWidth()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 1.25f)),
            UIComponentGridSizeData(UICoord(), 1.0f),
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 1.25f))
        });
        return s_data;
    }

    const std::vector<UIComponentGridSizeData>& GetUIGridSliderHorizontalHeight()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f))
        });
        return s_data;
    }

    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataHeaderBodyFooter()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 2.0f), 0.0f),
            UIComponentGridSizeData(UICoord(), 1.0f),
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 3.0f), 0.0f)
        });
        return s_data;
    }
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataThreeButtonsHorizontal()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_button_width), 0.0f),
            UIComponentGridSizeData(UICoord(), 1.0f),
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_button_width), 0.0f),
            UIComponentGridSizeData(UICoord(), 1.0f),
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_button_width), 0.0f)
        });
        return s_data;
    }
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataSmallBigHorizontal()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(), 1.0f),
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::X, 0.0f, s_default_margin), 0.0f),
            UIComponentGridSizeData(UICoord(), 2.0f),
        });
        return s_data;
    }

    typedef const UIBaseColour (*TGetUIBaseColour)(const int in_create_index);
    const UIBaseColour GetUIBaseColourDefault(const int)
    {
        return UIBaseColour();
    }
    const UIBaseColour GetUIBaseColourStaggerClearTransparent(const int in_create_index)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
            s_quick_fade * in_create_index,
            s_quick_fade
            );
    }
    const UIBaseColour GetUIBaseColourBlackSlowFade(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
            true,
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
            0.0f,
            s_slow_fade
            );
    }
    const UIBaseColour GetUIBaseColourStaggerClearDark(const int in_create_index)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.5f),
            true,
            VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
            s_quick_fade * in_create_index,
            s_quick_fade
            );
    }

    const UIBaseColour GetUIBaseColourRed(const int)
    {
        return UIBaseColour(
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f)
            );
    }
    const UIBaseColour GetUIBaseColourBlue(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f)
            );
    }
    const UIBaseColour GetUIBaseColourDark(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.5f),
            true
            );
    }
    const UIBaseColour GetUIBaseColourGrey(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f),
            true
            );
    }
    const UIBaseColour GetUIBaseColourWhite(const int)
    {
        return UIBaseColour(
            VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
            true
            );
    }

    const UIBaseColour GetUIBaseColourClearBlue(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f)
            );
    }
    const UIBaseColour GetUIBaseColourClearDark(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
            );
    }
    const UIBaseColour GetUIBaseColourClearLight(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(1.5f, 1.5f, 1.5f, 1.0f) //0.5f, 0.5f, 0.5f, 0.5f)
            );
    }
    const UIBaseColour GetUIBaseColourClearGrey(const int)
    {
        return UIBaseColour(
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
            true,
            VectorFloat4(0.75f, 0.75f, 0.75f, 1.0f) //0.5f, 0.5f, 0.5f, 0.5f)
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
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentCanvas* canvas = dynamic_cast<UIComponentCanvas*>(in_out_content.get());
        if (nullptr == canvas)
        {
            in_out_content = std::make_unique<UIComponentCanvas>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref());
            return true;
        }
        else
        {
            if (true == canvas->Set(
                in_get_base_colour(in_factory_param._create_index),
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
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentButton* content = dynamic_cast<UIComponentButton*>(in_out_content.get());
        if (nullptr == content)
        {
            in_out_content = std::make_unique<UIComponentButton>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref());
            return true;
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref()))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        UIEffectEnum in_effect_type = UIEffectEnum::TNone,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUICoordRef in_get_ui_coord_a = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_b = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_c = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_d = GetUICoordNone,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
        >
    const bool FactoryEffect(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        bool dirty = false;
        UIComponentEffect* content = dynamic_cast<UIComponentEffect*>(in_out_content.get());
        if (nullptr == content)
        {
            in_out_content = std::make_unique<UIComponentEffect>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_effect_type,
                in_get_ui_coord_a(),
                in_get_ui_coord_b(),
                in_get_ui_coord_c(),
                in_get_ui_coord_d(),
                in_get_ui_state_flag_tint_array()
                );
            dirty = true;
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_effect_type,
                in_get_ui_coord_a(),
                in_get_ui_coord_b(),
                in_get_ui_coord_c(),
                in_get_ui_coord_d(),
                in_get_ui_state_flag_tint_array()
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault
        >
    const bool FactoryDisable(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        bool dirty = false;
        UIComponentDisable* content = dynamic_cast<UIComponentDisable*>(in_out_content.get());
        if (nullptr == content)
        {
            in_out_content = std::make_unique<UIComponentDisable>(
                in_get_base_colour(in_factory_param._create_index),
                GetUILayoutShrink() //GetUILayout()
                );
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                GetUILayoutShrink())) //GetUILayout()))
            {
                dirty = true;
            }
        }

        return dirty;
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
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        auto font = in_factory_param._text_manager->GetTextFont(in_get_path_ref());
        const float new_line_gap_ratio = static_cast<float>(in_new_line_gap) / static_cast<float>(in_font_size);

        UIComponentString* content = dynamic_cast<UIComponentString*>(in_out_content.get());
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
            in_out_content = std::move(std::make_unique<UIComponentString>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(), 
                text_block
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(in_factory_param._create_index),
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
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentTextRun* content = dynamic_cast<UIComponentTextRun*>(in_out_content.get());
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
            in_out_content = std::move(std::make_unique<UIComponentTextRun>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(), 
                text_run
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(in_factory_param._create_index),
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
        UIOrientation in_orientation = UIOrientation::TVertical,
        TGetUICoordRef in_get_gap_ref = GetUICoordDefaultGap
        >
    const bool FactoryStack(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentStack* content = dynamic_cast<UIComponentStack*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentStack>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_orientation,
                in_get_gap_ref()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(in_factory_param._create_index),
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

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIGridSizeData in_get_grid_size_horizontal = GetUIGridSizeDataDefaultHorizontal,
        TGetUIGridSizeData in_get_grid_size_vertical = GetUIGridSizeDataDefaultVertical
        >
    const bool FactoryGrid(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentGrid* content = dynamic_cast<UIComponentGrid*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentGrid>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_grid_size_horizontal(),
                in_get_grid_size_vertical()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_grid_size_horizontal(),
                in_get_grid_size_vertical()
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
        UIOrientation in_orientation = UIOrientation::THorizontal
        >
    const bool FactorySlider(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentSlider* content = dynamic_cast<UIComponentSlider*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentSlider>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_orientation
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_orientation
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
    // UIData canvas
    in_ui_manager.AddContentFactory("UIData", FactoryCanvas<>);
    in_ui_manager.AddContentFactory("canvas_debug_quad0", FactoryCanvas<
        GetUILayoutQuadrant0, 
        GetUIBaseColourRed
        >);
    in_ui_manager.AddContentFactory("canvas_banner_left", FactoryCanvas<
        GetUILayoutBannerLeft, 
        GetUIBaseColourStaggerClearDark
        >);
    in_ui_manager.AddContentFactory("canvas_red", FactoryCanvas<
        GetUILayout, 
        GetUIBaseColourRed
        >);
    in_ui_manager.AddContentFactory("canvas_grey", FactoryCanvas<
        GetUILayout, 
        GetUIBaseColourGrey
        >);
    in_ui_manager.AddContentFactory("canvas_blue", FactoryCanvas<
        GetUILayout, 
        GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_margin", FactoryCanvas<
        GetUILayoutMargin
        >);
    in_ui_manager.AddContentFactory("canvas_margin_tiny", FactoryCanvas<
        GetUILayoutMarginTiny
        >);
    in_ui_manager.AddContentFactory("canvas_widget", FactoryCanvas<
        GetUILayoutWidget, 
        GetUIBaseColourDefault //GetUIBaseColourBlue
        >);

    // UIData stack
    in_ui_manager.AddContentFactory("stack", FactoryStack<>);

    in_ui_manager.AddContentFactory("stack_top_down", FactoryStack<
        GetUILayoutMarginTop
        >);

    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutMarginBottomRightShrink,
        GetUIBaseColourStaggerClearTransparent,
        UIOrientation::TVertical,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("stack_vertical_middle", FactoryStack<
        GetUILayoutMarginMiddleShrinkVertical,
        GetUIBaseColourStaggerClearTransparent,
        UIOrientation::TVertical,
        GetUICoordDefaultGap
        >);

    // UIData grid
    in_ui_manager.AddContentFactory("grid_dialog_header_body_footer", FactoryGrid<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIGridSizeDataDefaultHorizontal,
        GetUIGridSizeDataHeaderBodyFooter
        >);
    in_ui_manager.AddContentFactory("grid_three_buttons", FactoryGrid<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIGridSizeDataThreeButtonsHorizontal,
        GetUIGridSizeDataDefaultVertical
        >);

    in_ui_manager.AddContentFactory("grid_small_big_pair", FactoryGrid<
        GetUILayoutShrinkVertical,
        GetUIBaseColourDefault,
        GetUIGridSizeDataSmallBigHorizontal,
        GetUIGridSizeDataBigRowVertical
        >);

    in_ui_manager.AddContentFactory("grid_slider_horizontal", FactoryGrid<
        GetUILayoutSliderHorizontal,
        GetUIBaseColourDefault,
        GetUIGridSliderHorizontalWidth,
        GetUIGridSliderHorizontalHeight
        >);



    // UIData effect
    in_ui_manager.AddContentFactory("effect_debug", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TDebug
        >);
    in_ui_manager.AddContentFactory("effect_drop_shadow", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TDropShadow,
        GetUIBaseColourClearDark,
        GetUICoordDefaultGapQuater,
        GetUICoordDefaultGapHalf,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_inner_shadow", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TInnerShadow,
        GetUIBaseColourClearDark
        >);
    in_ui_manager.AddContentFactory("effect_drop_glow", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TDropShadow,
        GetUIBaseColourClearLight,
        GetUICoordNone,
        GetUICoordNone,
        GetUICoordDefaultGap
        >);

    in_ui_manager.AddContentFactory("effect_corner", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TRoundCorners,
        GetUIBaseColourDefault,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_corner_modal", FactoryEffect<
        GetUILayoutModal,
        UIEffectEnum::TRoundCorners,
        GetUIBaseColourDefault,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_corner_tiny", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TRoundCorners,
        GetUIBaseColourDefault,
        GetUICoordDefaultGapQuater,
        GetUICoordDefaultGapQuater,
        GetUICoordDefaultGapQuater,
        GetUICoordDefaultGapQuater
        >);


    in_ui_manager.AddContentFactory("effect_gloss", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TGloss,
        GetUIBaseColourDefault
        >);
    in_ui_manager.AddContentFactory("effect_fill", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TFill,
        GetUIBaseColourClearBlue,
        GetUICoordDefaultGapHalf,
        GetUICoordNone,
        GetUICoordNone,
        GetUICoordNone,
        GetUIStateFlagTintArrayDefault
        >);

    in_ui_manager.AddContentFactory("UIDataDisable", FactoryDisable<
        GetUIBaseColourClearGrey
        >);

    // UIDataString
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
    in_ui_manager.AddContentFactory("string_middle", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::Middle
        >);
    in_ui_manager.AddContentFactory("string_right", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::Middle
        >);

    in_ui_manager.AddContentFactory("string_right_em", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap_small,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_small_right_em", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetFontPathDefault,
        s_default_font_size_small,
        s_new_line_gap_small,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_small_right_fixed", FactoryString<
        GetUILayoutBottomRightShrink,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetFontPathFixedWidth,
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
        GetUIBaseColourDefault //GetUIBaseColourRed
        >);
    in_ui_manager.AddContentFactory("button_default", FactoryButton<
        GetUILayout, 
        GetUIBaseColourDefault //GetUIBaseColourRed
        >);
    in_ui_manager.AddContentFactory("button_background", FactoryButton<
        GetUILayout, 
        GetUIBaseColourDark
        >);
    in_ui_manager.AddContentFactory("button_modal_body", FactoryButton<
        GetUILayout, //GetUILayoutModal, 
        GetUIBaseColourGrey
        >);

    in_ui_manager.AddContentFactory("UIDataToggle", FactoryButton<
        GetUILayoutCheckboxLeft, 
        GetUIBaseColourGrey
        >);

    // button of a slider
    in_ui_manager.AddContentFactory("button_widget", FactoryButton<
        GetUILayoutWidget, 
        GetUIBaseColourGrey
        >);

    in_ui_manager.AddContentFactory("UIDataFloat", FactorySlider<>);
    in_ui_manager.AddContentFactory("slider_horizontal", FactorySlider<
        GetUILayoutSliderHorizontal
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

