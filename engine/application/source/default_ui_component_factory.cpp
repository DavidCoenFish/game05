#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/vector_int4.h"
#include "common/text/text_manager.h"
#include "common/text/text_enum.h"
#include "common/text/text_font.h"
#include "common/text/text_block.h"
#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_stack.h"
#include "common/ui/ui_content_text.h"
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

    void FactoryCanvas(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam&
        )
    {
        UIContentCanvas* canvas = dynamic_cast<UIContentCanvas*>(in_out_content.get());
        if (nullptr == canvas)
        {
            in_out_content = std::make_unique<UIContentCanvas>();
        }
        return;
    }

    template<
        TGetPath GetPath,
        int FontSize = 16,
        TextEnum::HorizontalLineAlignment Horizontal = TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment Vertical = TextEnum::VerticalBlockAlignment::MiddleEM
        >
    //DrawSystem* const _draw_system;
    //ID3D12GraphicsCommandList* const _command_list;
    //const IUIData* const _ui_data;
    //TextManager* const _text_manager;

    void FactoryString(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
        UIDataString* const data = dynamic_cast<UIDataString*>(in_param._ui_data);
        if (nullptr == data)
        {
            in_out_content.reset();
            return true;
        }
        bool dirty = false;
        UIContentText* text = dynamic_cast<UIContentText*>(in_out_content.get());
        if (nullptr == text)
        {

            auto text_unique = std::make_unique<UIContentText>();
            text = text_unique.get();
            in_out_content = std::move(text_unique);
            dirty = true;
        }

        text->SetText(data->GetString());
        text->SetAlignment(Horizontal, Vertical);
        text->SetFont(
            in_param._text_manager->MakeFont(GetPath()),
            FontSize
            );


        //if (true == text->FactoryUpdate(
        //    GetPath(),
        //    data,
        //    FontSize,
        //    Horizontal,
        //    Vertical
        //    ))
        //{
        //    dirty = true;
        //}
    }

    template<
        int FontSize = 16
        >
    void FactoryTextRun(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
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
    void FactoryStack(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
    }
}
void DefaultUIComponentFactory::Populate(
    UIManager& in_ui_manager
    )
{
    in_ui_manager.AddContentFactory("canvas", FactoryCanvas);
    in_ui_manager.AddContentFactory("UIDataString", FactoryString<>);
    in_ui_manager.AddContentFactory("UIDataTextRun", FactoryTextRun<>);
    in_ui_manager.AddContentFactory("UIDataContainer", FactoryStack<>);
    in_ui_manager.AddContentFactory("stack_vertical_bottom_right", FactoryStack<
        StackOrientation::TVertical,
        AlignmentHorizontal::TRight,
        AlignmentVertical::TBottom
        >);
    //UIDataTexture
    //UIDataButton
    //UIDataIconButton
}
