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

    enum class StackOrientation
    {
        TVertical,
        THorizontal,
        //TGridLeftRightTopBottom?
        //TGridTopBottomLeftRight?
        //TFlowLeftRightTopBottom
        //TFlowTopBottomLeftRight
    };

    typedef const std::filesystem::path (*TGetPath)();

    const bool FactoryCanvas(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentCanvas* canvas = dynamic_cast<UIContentCanvas*>(in_out_content.get());
        if (nullptr == canvas)
        {
            in_out_content = std::make_unique<UIContentCanvas>();
            return true;
        }
        return false;
    }

    const std::filesystem::path GetDeafuleFontPath()
    {
        return std::filesystem::path("data") / "code2000.ttf"; //"deja_vu_sans.ttf"; //"open_sans.ttf";
    }

    template<
        TGetPath GetPath = GetDeafuleFontPath,
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

        UIContentString* string = dynamic_cast<UIContentString*>(in_out_content.get());
        bool dirty = false;
        if (nullptr == string)
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
            in_out_content = std::move(std::make_unique<UIContentString>(text_block));
            dirty = true;
        }
        else
        {
            if (true == string->SetFont(*font, FontSize, NewLineHeight))
            {
                dirty = true;
            }

            if (true == string->SetAlignment(Horizontal, Vertical, EMSize))
            {
                dirty = true;
            }
        }

        return dirty;
    }

    template<
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
        StackOrientation Orientation = StackOrientation::TVertical, 
        AlignmentHorizontal Horizontal = AlignmentHorizontal::TMiddle, 
        AlignmentVertical Vertical = AlignmentVertical::TMiddle, 
        bool ShrinkToFit = false, 
        int MarginLeft = 8,
        int MarginRight = 8,
        int MarginTop = 8,
        int MarginBottom = 8
        >
    const bool FactoryStack(
        std::unique_ptr<IUIContent>&, //in_out_content,
        const UIContentFactoryParam& //in_param
        )
    {
        return false;
    }
}
void DefaultUIComponentFactory::Populate(
    UIManager& in_ui_manager
    )
{
    //in_ui_manager.AddContentFactory("", FactoryDefault); // Make this built in to UIManager?
    in_ui_manager.AddContentFactory("UIDataString", FactoryString<>);
    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<>);
    in_ui_manager.AddContentFactory("UIDataContainer", FactoryCanvas);
    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        StackOrientation::TVertical,
        AlignmentHorizontal::TRight,
        AlignmentVertical::TBottom
        >);
    //UIDataTexture
    //UIDataButton
    //UIDataIconButton
}
