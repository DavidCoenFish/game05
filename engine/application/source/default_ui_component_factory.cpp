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
#include "common/ui/ui_component/ui_component_drift.h"
#include "common/ui/ui_component/ui_component_effect.h"
#include "common/ui/ui_component/ui_component_grid.h"
#include "common/ui/ui_component/ui_component_list_box.h"
#include "common/ui/ui_component/ui_component_manual_scroll.h"
#include "common/ui/ui_component/ui_component_scroll.h"
#include "common/ui/ui_component/ui_component_slider.h"
#include "common/ui/ui_component/ui_component_stack.h"
#include "common/ui/ui_component/ui_component_string.h"
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
#include "common/ui/ui_data/ui_data_scroll.h"
#include "common/ui/ui_data/ui_data_list_box.h"
#include "common/ui/ui_data/ui_data_manual_scroll.h"

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
    const UILayout& GetUILayoutMarginListbox()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-1.875f)),// (-1.75f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-1.875f)),// (-1.75f)),
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
            UICoord(UICoord::ParentSource::X, 1.0f, -(s_default_font_size * 0.375f)), //s_default_margin * (-0.25f)),
            UICoord(UICoord::ParentSource::Y, 1.0f, -(s_default_font_size * 0.375f)), //s_default_margin * (-0.25f)),
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
    const UILayout& GetUILayoutTopSideMargin()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            false,
            true
            );
        return s_layout;
    }

    const UILayout& GetUILayoutTop()
    {
        // Todo: is y up the screen for ui, y==0 is bottom, y==1 is top screen
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 1.0f),
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
    const UILayout& GetUILayoutListBoxRow()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size),
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
    const UILayout& GetUILayoutCheckbox()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size), // 1.1f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
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

    const UILayout& GetUILayoutKnot()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 1.0f), // 1.1f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutCheckboxWrapper()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_font_size * 2.0f), // 1.1f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 2.0f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutScrollWrapper()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutManualScrollHorizontal()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.0f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutManualScrollHorizontalLong()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.0f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutManualScrollVertical()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::Y, 1.0f, s_default_margin * (-2.0f)),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutManualScrollVerticalLong()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }
    const UILayout& GetUILayoutDebugScrollVertical()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 0.0f, s_default_margin),
            UICoord(UICoord::ParentSource::Y, 0.0f, 100.0f),
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 1.0f),
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
    const UILayout& GetUILayoutGridSliderHorizontal()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, -(s_default_font_size * 0.375f)),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 1.25f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutHorizontalRule()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 0.125f), // 1.1f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutListBoxWrapper()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f),
            UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 8.0f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.5f),
            UICoord(UICoord::ParentSource::Y, 0.5f)
            );
        return s_layout;
    }

    const UILayout& GetUILayoutStackListBox()
    {
        static UILayout s_layout(
            UICoord(UICoord::ParentSource::X, 1.0f, -s_default_margin),
            UICoord(UICoord::ParentSource::Y, 1.0f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            UICoord(UICoord::ParentSource::X, 0.0f),
            UICoord(UICoord::ParentSource::Y, 0.5f),
            false,
            true
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
    const UICoord& GetUICoordDefaultGapSmall()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin * 0.75f);
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
    const UICoord& GetUICoordDefaultGapEigth()
    {
        static UICoord s_coord(UICoord::ParentSource::None, 0.0f, s_default_margin * 0.125f);
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
            VectorFloat4(0.0f, 0.8f, 0.7f, 1.0f), // TTouch
            VectorFloat4(0.0f, 0.4f, 0.9f, 1.0f), // TTouch THover
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f), // TSelected
            VectorFloat4(1.0f, 0.2f, 0.2f, 1.0f), // TSelected THover
            VectorFloat4(0.7f, 0.8f, 0.0f, 1.0f), // TSelected TTouch
            VectorFloat4(0.9f, 0.4f, 0.0f, 1.0f), // TSelected TTouch THover

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
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 2.0f))
        });
        return s_data;
    }
    const std::vector<UIComponentGridSizeData>& GetUIGridSizeDataBigRowVerticalTall()
    {
        static std::vector<UIComponentGridSizeData> s_data({
            UIComponentGridSizeData(UICoord(UICoord::ParentSource::Y, 0.0f, s_default_font_size * 8.0f))
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
    /// Default is transparent with a white tint
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
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
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
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                );
            return true;
        }
        else
        {
            if (true == canvas->Set(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                ))
            {
                return true;
            }
        }

        return false;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
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
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                );
            return true;
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                ))
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
        TGetUICoordRef in_get_ui_coord_a = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_b = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_c = GetUICoordNone,
        TGetUICoordRef in_get_ui_coord_d = GetUICoordNone
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
                in_get_ui_state_flag_tint_array(),
                in_effect_type,
                in_get_ui_coord_a(),
                in_get_ui_coord_b(),
                in_get_ui_coord_c(),
                in_get_ui_coord_d()
                );
            dirty = true;
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_effect_type,
                in_get_ui_coord_a(),
                in_get_ui_coord_b(),
                in_get_ui_coord_c(),
                in_get_ui_coord_d()
                ))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
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
                GetUILayoutShrink(), //GetUILayout()
                GetUIStateFlagTintArray()
                );
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                GetUILayoutShrink(), //GetUILayout()))
                GetUIStateFlagTintArray()))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
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
                in_get_ui_state_flag_tint_array(),
                text_block
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(in_factory_param._create_index),
                    in_get_layout_ref(),
                    in_get_ui_state_flag_tint_array()))
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
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
                in_get_ui_state_flag_tint_array(),
                text_run
                ));
            dirty = true;
        }
        else
        {
            if (true == content->SetBase(
                    in_get_base_colour(in_factory_param._create_index),
                    in_get_layout_ref(),
                    in_get_ui_state_flag_tint_array()
                    ))
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
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
                in_get_ui_state_flag_tint_array(),
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
                in_get_ui_state_flag_tint_array(),
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
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
                in_get_ui_state_flag_tint_array(),
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
                in_get_ui_state_flag_tint_array(),
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
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
                in_get_ui_state_flag_tint_array(),
                in_orientation
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_orientation
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray,
        UIOrientation in_orientation = UIOrientation::THorizontal
        >
    const bool FactoryScroll(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentScroll* content = dynamic_cast<UIComponentScroll*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentScroll>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_orientation
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_orientation
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
        >
    const bool FactoryManualScroll(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentManualScroll* content = dynamic_cast<UIComponentManualScroll*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentManualScroll>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
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
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
        >
    const bool FactoryListBox(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        UIComponentListBox* content = dynamic_cast<UIComponentListBox*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentListBox>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->SetBase(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array()
                ))
            {
                dirty = true;
            }
        }
        return dirty;
    }

    template<
        UISlideDirection in_direction = UISlideDirection::TFromLeft,
        int in_duration_times_1000 = 1000,
        TGetUILayoutRef in_get_layout_ref = GetUILayout,
        TGetUIBaseColour in_get_base_colour = GetUIBaseColourDefault,
        TGetUIStateFlagTintArray in_get_ui_state_flag_tint_array = GetUIStateFlagTintArray
        >
    const bool FactoryDrift(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_factory_param
        )
    {
        const float duration = static_cast<float>(in_duration_times_1000) / 1000.0f;
        UIComponentDrift* content = dynamic_cast<UIComponentDrift*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == content)
        {
            dirty = true;
            auto new_content = std::make_unique<UIComponentDrift>(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_direction,
                duration
                );
            in_out_content = std::move(new_content);
        }
        else
        {
            if (true == content->Set(
                in_get_base_colour(in_factory_param._create_index),
                in_get_layout_ref(),
                in_get_ui_state_flag_tint_array(),
                in_direction,
                duration
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
    in_ui_manager.AddContentFactory("canvas_white", FactoryCanvas<
        GetUILayout, 
        GetUIBaseColourWhite
        >);
    in_ui_manager.AddContentFactory("horizontal_rule", FactoryCanvas<
        GetUILayoutHorizontalRule, 
        GetUIBaseColourGrey
        >);
    in_ui_manager.AddContentFactory("canvas_blue", FactoryCanvas<
        GetUILayout, 
        GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_margin", FactoryCanvas<
        GetUILayoutMargin
        >);
    in_ui_manager.AddContentFactory("canvas_margin_listbox", FactoryCanvas<
        GetUILayoutMarginListbox
        >);
    in_ui_manager.AddContentFactory("canvas_margin_tiny", FactoryCanvas<
        GetUILayoutMarginTiny
        >);
    in_ui_manager.AddContentFactory("canvas_knot", FactoryCanvas<
        GetUILayoutKnot, 
        GetUIBaseColourDefault //GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_checkbox_wrapper", FactoryCanvas<
        GetUILayoutCheckboxWrapper, 
        GetUIBaseColourDefault //GetUIBaseColourBlue
        >);

    in_ui_manager.AddContentFactory("canvas_scroll_horizontal_wrapper", FactoryCanvas<
        GetUILayoutScrollWrapper, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);

    in_ui_manager.AddContentFactory("canvas_manual_scroll_horizontal", FactoryCanvas<
        GetUILayoutManualScrollHorizontal, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_manual_scroll_vertical", FactoryCanvas<
        GetUILayoutManualScrollVertical, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_manual_scroll_horizontal_long", FactoryCanvas<
        GetUILayoutManualScrollHorizontalLong, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);
    in_ui_manager.AddContentFactory("canvas_manual_scroll_vertical_long", FactoryCanvas<
        GetUILayoutManualScrollVerticalLong, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);


    in_ui_manager.AddContentFactory("canvas_debug_scroll_vertical", FactoryCanvas<
        GetUILayoutDebugScrollVertical, 
        GetUIBaseColourDark //GetUIBaseColourBlue
        >);

    in_ui_manager.AddContentFactory("canvas_list_box_wrapper", FactoryCanvas<
        GetUILayoutListBoxWrapper, 
        GetUIBaseColourRed
        >);

    in_ui_manager.AddContentFactory("canvas_row", FactoryCanvas<
        GetUILayoutRow, 
        GetUIBaseColourDefault
        >);

    // UIData stack
    in_ui_manager.AddContentFactory("stack", FactoryStack<>);

    in_ui_manager.AddContentFactory("stack_top_down", FactoryStack<
        GetUILayoutTopSideMargin,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        UIOrientation::TVertical,
        GetUICoordNone // No gap
        >);

    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        GetUILayoutMarginBottomRightShrink,
        GetUIBaseColourStaggerClearTransparent,
        GetUIStateFlagTintArray,
        UIOrientation::TVertical,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("stack_vertical_middle", FactoryStack<
        GetUILayoutMarginMiddleShrinkVertical,
        GetUIBaseColourStaggerClearTransparent,
        GetUIStateFlagTintArray,
        UIOrientation::TVertical,
        GetUICoordDefaultGap
        >);

    in_ui_manager.AddContentFactory("stack_list_box", FactoryStack<
        GetUILayoutStackListBox,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        UIOrientation::TVertical,
        GetUICoordNone
        >);

    // UIData grid
    in_ui_manager.AddContentFactory("grid_dialog_header_body_footer", FactoryGrid<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUIGridSizeDataDefaultHorizontal,
        GetUIGridSizeDataHeaderBodyFooter
        >);
    in_ui_manager.AddContentFactory("grid_three_buttons", FactoryGrid<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUIGridSizeDataThreeButtonsHorizontal,
        GetUIGridSizeDataDefaultVertical
        >);

    in_ui_manager.AddContentFactory("grid_small_big_pair", FactoryGrid<
        GetUILayoutShrinkVertical,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUIGridSizeDataSmallBigHorizontal,
        GetUIGridSizeDataBigRowVertical
        >);

    in_ui_manager.AddContentFactory("grid_small_big_pair_tall", FactoryGrid<
        GetUILayoutShrinkVertical,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUIGridSizeDataSmallBigHorizontal,
        GetUIGridSizeDataBigRowVerticalTall
        >);

    in_ui_manager.AddContentFactory("grid_slider_horizontal", FactoryGrid<
        GetUILayoutGridSliderHorizontal,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
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
        GetUIStateFlagTintArray,
        GetUICoordDefaultGapQuater,
        GetUICoordDefaultGapHalf,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_drop_shadow_small", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TDropShadow,
        GetUIBaseColourClearDark,
        GetUIStateFlagTintArray,
        GetUICoordDefaultGapEigth,
        GetUICoordDefaultGapEigth,
        GetUICoordDefaultGapQuater
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
        GetUIStateFlagTintArray,
        GetUICoordNone,
        GetUICoordNone,
        GetUICoordDefaultGap
        >);

    in_ui_manager.AddContentFactory("effect_corner", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TRoundCorners,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_corner_modal", FactoryEffect<
        GetUILayoutModal,
        UIEffectEnum::TRoundCorners,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap,
        GetUICoordDefaultGap
        >);
    in_ui_manager.AddContentFactory("effect_corner_hollow_tiny", FactoryEffect<
        GetUILayout,
        UIEffectEnum::TRoundCornersHollow,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetUICoordDefaultGapHalf,
        GetUICoordDefaultGapHalf,
        GetUICoordDefaultGapHalf,
        GetUICoordDefaultGapHalf
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
        GetUIStateFlagTintArrayDefault,
        GetUICoordDefaultGapHalf,
        GetUICoordNone,
        GetUICoordNone,
        GetUICoordNone
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
        GetUIStateFlagTintArray,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_middle", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::Middle
        >);
    in_ui_manager.AddContentFactory("string_right", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::Middle
        >);

    in_ui_manager.AddContentFactory("string_right_em", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetUIStateFlagTintArray,
        GetFontPathDefault,
        s_default_font_size,
        s_new_line_gap_small,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_small_right_em", FactoryString<
        GetUILayout,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetUIStateFlagTintArray,
        GetFontPathDefault,
        s_default_font_size_small,
        s_new_line_gap_small,
        TextEnum::HorizontalLineAlignment::Right,
        TextEnum::VerticalBlockAlignment::MiddleEM
        >);
    in_ui_manager.AddContentFactory("string_small_right_fixed", FactoryString<
        GetUILayoutBottomRightShrink,
        GetUIBaseColourDefault, //GetUIBaseColourDebugRed, //
        GetUIStateFlagTintArray,
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
        GetUIStateFlagTintArray,
        s_default_font_size_small,
        TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment::Top
        >);
    in_ui_manager.AddContentFactory("text_run_middle", FactoryTextRun<
        GetUILayoutShrink
        >);
    in_ui_manager.AddContentFactory("UIDataTextRunWrap", FactoryTextRun<
        GetUILayoutQuadrant0,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
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
    in_ui_manager.AddContentFactory("button_listbox_item", FactoryButton<
        GetUILayout, 
        GetUIBaseColourWhite,
        GetUIStateFlagTintArrayDefault
        >);
    in_ui_manager.AddContentFactory("UIDataToggle", FactoryButton<
        GetUILayoutCheckbox, 
        GetUIBaseColourDefault
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

    in_ui_manager.AddContentFactory("UIDataScroll", FactoryScroll<>);
    in_ui_manager.AddContentFactory("scroll_horizontal", FactoryScroll<>);
    in_ui_manager.AddContentFactory("scroll_vertical", FactoryScroll<
        GetUILayout,
        GetUIBaseColourDefault,
        GetUIStateFlagTintArray,
        UIOrientation::TVertical
        >);

    in_ui_manager.AddContentFactory("UIDataManualScroll", FactoryManualScroll<>);
    in_ui_manager.AddContentFactory("UIDataListBox", FactoryListBox<>);
    in_ui_manager.AddContentFactory("drift_from_top", FactoryDrift<
        UISlideDirection::TFromTop,
        1000
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

