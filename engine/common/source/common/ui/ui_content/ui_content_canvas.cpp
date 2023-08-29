#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIContentCanvas::UIContentCanvas()
{
    // Nop
}

UIContentCanvas::~UIContentCanvas()
{
    // Nop
}

const bool UIContentCanvas::Update(
    VectorInt2& out_texture_size,
    UIHierarchyNodeLayoutData& out_layout_data,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
    const VectorInt2& in_parent_size,
    const IUIData* const in_data,
    const std::string& in_model_key,
    const UIHierarchyNodeUpdateLayoutParam& in_param
    )
{
    const UIDataContainer* const data_container = dynamic_cast<const UIDataContainer*>(in_data);
    if (nullptr != data_container)
    {
        const auto& container = data_container->GetDataConst();
        const int count = container.size();
        out_child_data_array.resize(count);
        for (int index = 0; index < count; ++index)
        {
            auto& child_data = out_child_data_array[index];
            auto& item = container[index];
            if (nullptr == item)
            {
                child_data.reset();
            }
            else
            {
                if (nullptr == child_data)
                {
                    child_data = std::make_shared<UIHierarchyNodeChildData>(
                        std::make_shared<UIHierarchyNode>(),
                        std::make_shared<UIGeometry>(
                            in_param._draw_system,
                            in_param._command_list
                            )
                        );
                }

                child_data->_node->UpdateLayout(
                    item.get(),
                    in_model_key,
                    true,
                    false,
                    in_parent_size,
                    in_param
                    );
            }
        }
    }
    else
    {
        out_child_data_array.empty();
    }
}