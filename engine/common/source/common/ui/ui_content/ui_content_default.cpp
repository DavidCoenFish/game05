#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_default.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_manager.h"

UIContentDefault::UIContentDefault()
: _source_ui_data_token(nullptr)
{
    // Nop
}

UIContentDefault::~UIContentDefault()
{
    // Nop
}

const bool UIContentDefault::GetClearBackground(
    VectorFloat4&
    ) const
{
    return false;
}
//
//// Default implementation is to clear the children, or do we add a virtual get array UIData and make children based on that?
//const bool UIContentDefault::UpdateHierarchy(
//    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
//    const IUIData&,
//    const UIHierarchyNodeUpdateHierarchyParam& in_param
//    ) const
//{
//    bool dirty = false;
//    const std::vector<std::shared_ptr<IUIData>>& array_data = GetArrayData();
//    const int count = (int)array_data.size();
//    dirty = (int)out_child_data_array.size() != count;
//    out_child_data_array.resize(count);
//    for (int index = 0; index < count; ++index)
//    {
//        if (nullptr == out_child_data_array[index])
//        {
//            out_child_data_array[index] = std::make_shared<UIHierarchyNodeChildData>(
//                std::make_shared<UIHierarchyNode>(),
//                std::make_shared<UIGeometry>()
//                );
//        }
//
//        if (true == out_child_data_array[index]->_node->UpdateHierarchy(
//            in_param,
//            *array_data[index],
//            true,
//            false
//            ))
//        {
//            dirty = true;
//        }
//    }
//    return dirty;
//}
//
//const std::vector<std::shared_ptr<IUIData>>& UIContentDefault::GetArrayData() const
//{
//    static std::vector<std::shared_ptr<IUIData>> s_empty;
//    return s_empty;
//}
//

void* UIContentDefault::GetSourceUIDataToken() const
{
    return _source_ui_data_token;
}

const bool UIContentDefault::UpdateHierarchy(
    const IUIData* const in_data,
    UIHierarchyNodeChildData& , //in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& //in_param
    )
{
    bool dirty = false;
    if (_source_ui_data_token == (void*)in_data)
    {
        dirty = true;
        _source_ui_data_token = (void*)in_data;
    }
    return dirty;
}

void UIContentDefault::Draw(
    const UIManagerDrawParam& in_param,
    Shader* const in_shader,
    UIGeometry* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item // The texture resource of the node for this content
    )
{
    in_shader->SetShaderResourceViewHandle(0, in_heap_wrapper_item);
    auto geometry = in_geometry->GetGeometry(
        in_param._draw_system,
        in_param._frame->GetCommandList()
        );

    in_param._frame->SetShader(in_shader);
    in_param._frame->Draw(geometry);

    return;
}
