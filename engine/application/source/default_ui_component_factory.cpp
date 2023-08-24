#include "application_pch.h"
#include "default_ui_component_factory.h"

#include "common/macro.h"
#include "common/math/vector_int4.h"
#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_stack.h"
#include "common/ui/ui_content_text.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"

namespace
{
/*
    std::shared_ptr<UIHierarchyNode> FactoryBackgroundInfo(
            UIManager* const in_ui_manager,
            DrawSystem* const in_draw_system,
            ID3D12GraphicsCommandList* const in_command_list,
            IUIProviderData* const ,//in_provider_data,
            const std::string& in_key_base
            )
    {
        in_key_base;
        DSC_TODO("generate content for stack");

        //stack panel in bottom right corner, data source "build_info"
        auto node = std::make_shared<UIHierarchyNode>(
            UIHierarchyNode::LayoutData::LayoutData(
                UICoord(1.0f, -2.0f),
                UICoord(1.0f, -2.0f),
                UICoord(1.0f, -1.0f),
                UICoord(0.0f, 1.0f),
                UICoord(1.0f, 0.0f),
                UICoord(0.0f, 0.0f)
                ),
            UIHierarchyNode::MakeContentStack(),
            UIHierarchyNode::MakeTextureRenderTarget(
                in_draw_system,
                in_command_list
                )
            );

        return node;
    }
*/
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
        int FontSize = 16,
        AlignmentHorizontal Horizontal = AlignmentHorizontal::TMiddle, 
        AlignmentVertical Vertical = AlignmentVertical::TMiddle
        // function pointer to get default face?
        >
    void FactoryString(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )
    {
        bool dirty = false;
        UIContentText* text = dynamic_cast<UIContentText*>(in_out_content.get());
        if (nullptr == text)
        {
            in_out_content = std::make_unique<UIContentText>();
            dirty = true;
        }
        // do we have the right face
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
