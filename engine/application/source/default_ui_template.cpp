#include "application_pch.h"
#include "default_ui_template.h"

#include "common/macro.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_hierarchy_node.h"

namespace
{
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
}

void DefaultUITemplate::Populate(
    UIManager& in_ui_manager
    )
{
    in_ui_manager.AddHierarchyNodeFactory("background_info", FactoryBackgroundInfo);
}
