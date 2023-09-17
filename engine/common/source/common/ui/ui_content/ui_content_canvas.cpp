#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIContentCanvas::UIContentCanvas(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
{
    // Nop
}

UIContentCanvas::~UIContentCanvas()
{
    // Nop
}

const bool UIContentCanvas::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    return _content_default.SetBase(
        in_clear_background,
        in_clear_colour,
        in_layout
        );
}

void UIContentCanvas::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentCanvas::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentCanvas::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentCanvas::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    const IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        );
}

void UIContentCanvas::UpdateSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_parent_size,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node
        );
}

const VectorInt2 UIContentCanvas::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
    return _content_default.GetDesiredSize(
        in_parent_size,
        in_ui_scale
        );
}

void UIContentCanvas::Draw(
    const UIManagerDrawParam& in_param,
    Shader* const in_shader,
    UIGeometry* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
    )
{
    return _content_default.Draw(
        in_param,
        in_shader,
        in_geometry,
        in_heap_wrapper_item
        );
}


/*
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
        const int count = (int)container.size();
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
*/