#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_stack.h"
#include "common/ui/ui_content_text.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

std::unique_ptr<IUIContent> UIHierarchyNode::MakeContentCanvas(
    // Children source callback
    )
{
    return std::make_unique<UIContentCanvas>();
}

std::unique_ptr<IUIContent> UIHierarchyNode::MakeContentStack(
    // Horizontal or vertical
    // Margin for children
    // Children source callback
    )
{
    return std::make_unique<UIContentStack>();
}

std::unique_ptr<IUIContent> UIHierarchyNode::MakeContentText(
    std::shared_ptr<TextBlock>& in_text_block
    )
{
    return std::make_unique<UIContentText>(in_text_block);
}

std::unique_ptr<UITexture> UIHierarchyNode::MakeTextureBackBuffer(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    return std::make_unique<UITexture>(
        in_draw_system,
        in_command_list
        );
}

std::unique_ptr<UITexture> UIHierarchyNode::MakeTextureRenderTarget(
    DrawySystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size
    )
{
    return std::make_unique<UITexture>(
        in_draw_system,
        in_command_list,
        in_size,
        false
        );
}

VectorFloat4 UIHierarchyNode::CalculatePos(
    const VectorInt2& in_parent_size,
    const LayoutData& in_layout_data,
    const float in_ui_scale
    )
{
    const float x = (float)in_parent_size.GetX();
    const float y = (float)in_parent_size.GetY();

    //UICoord _data_size[2];
    const float sizeX = in_layout_data._data_size[0].Calculate(x, in_ui_scale);
    const float sizeY = in_layout_data._data_size[1].Calculate(y, in_ui_scale);

    //UICoord _data_attach[2];
    const float attachX = in_layout_data._data_attach[0].Calculate(x, in_ui_scale);
    const float attachY = in_layout_data._data_attach[1].Calculate(y, in_ui_scale);

    //UICoord _data_pivot[2];
    const float pivotX = in_layout_data._data_pivot[0].Calculate(sizeX, in_ui_scale);
    const float pivotY = in_layout_data._data_pivot[1].Calculate(sizeY, in_ui_scale);

    const float childX = attachX - pivotX;
    const float childY = attachY - pivotY;

    return VectorFloat4(
        childX,
        childY,
        childX + sizeX,
        childY + sizeY
        );
}

VectorInt2 UIHierarchyNode::CalculateSizeInt(
    const VectorInt2& in_parent_size,
    const LayoutData& in_layout_data,
    const float in_ui_scale
    )
{
    const float x = (float)in_parent_size.GetX();
    const float y = (float)in_parent_size.GetY();

    //UICoord _data_size[2];
    const int sizeX = (int)ceil(in_layout_data._data_size[0].Calculate(x, in_ui_scale));
    const int sizeY = (int)ceil(in_layout_data._data_size[1].Calculate(y, in_ui_scale));

    return VectorInt2(
        sizeX,
        sizeY
        );
}

UIHierarchyNode::UIHierarchyNode(
    const LayoutData& in_layout_data,
    std::unique_ptr<IUIContent>& in_content,
    std::unique_ptr<UITexture>& in_texture,
    const TFlag in_flag
    )
    : _layout_data(in_layout_data)
    , _content(std::move(in_content))
    , _texture(std::move(in_texture))
    , _flag(in_flag)
{
    // Nop
}

UIHierarchyNode::~UIHierarchyNode()
{
    // Nop
}

const VectorInt2 UIHierarchyNode::GetSize() const
{
    return _texture->GetSize();
}

//return true if all bits of flga are set
const bool UIHierarchyNode::GetFlag(const TFlag in_flag)
{
    if (((int)_flag & (int)in_flag) == (int)in_flag)
    {
        return true;
    }
    return false;
}

void UIHierarchyNode::SetFlag(const TFlag in_flag, const bool in_value)
{
    if (true == in_value)
    {
        _flag = (TFlag)((int)_flag | (int)in_flag);
    }
    else
    {
        _flag = (TFlag)((int)_flag & ~(int)in_flag);
    }
    return;
}

void UIHierarchyNode::AddChild(
    std::unique_ptr<UIHierarchyNode>& in_node,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    auto child = std::make_shared<ChildData>(
        std::move(in_node),
        std::make_unique<UIGeometry>(
            in_draw_system,
            in_command_list
            )
        );
    _child_data_array.push_back(child);
}

void UIHierarchyNode::ClearChildren()
{
    _child_data_array.clear();
}