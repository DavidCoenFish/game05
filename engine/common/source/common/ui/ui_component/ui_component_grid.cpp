#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_grid.h"

#include "common/math/vector_int4.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentGridSizeData::UIComponentGridSizeData(
    const UICoord& in_size,
    const float in_remainder_bias
    )
    : _size(in_size)
    , _remainder_bias(in_remainder_bias)
{
    // Nop
}

const bool UIComponentGridSizeData::operator==(const UIComponentGridSizeData& in_rhs) const
{
    if (_size != in_rhs._size)
    {
        return false;
    }
    if (_remainder_bias != in_rhs._remainder_bias)
    {
        return false;
    }
    return true;
}

const bool UIComponentGridSizeData::operator!=(const UIComponentGridSizeData& in_rhs) const
{
    return !(operator==(in_rhs));
}

UIComponentGrid::UIComponentGrid(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::vector<UIComponentGridSizeData>& in_horizontal_size_array,
    const std::vector<UIComponentGridSizeData>& in_vertical_size_array
    )
    : _component_default(in_base_colour, in_layout)
    , _horizontal_size_array(in_horizontal_size_array)
    , _vertical_size_array(in_vertical_size_array)
{
    // Mop
}

UIComponentGrid::~UIComponentGrid()
{
    // Nop
}

const bool UIComponentGrid::Set(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::vector<UIComponentGridSizeData>& in_horizontal_size_array,
    const std::vector<UIComponentGridSizeData>& in_vertical_size_array
    )
{
    bool dirty = false;

    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout
        ))
    {
        dirty = true;
    }

    if (_horizontal_size_array != in_horizontal_size_array)
    {
        dirty = true;
        _horizontal_size_array = in_horizontal_size_array;
    }

    if (_vertical_size_array != in_vertical_size_array)
    {
        dirty = true;
        _vertical_size_array = in_vertical_size_array;
    }

    return dirty;
}

const bool UIComponentGrid::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const bool UIComponentGrid::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentGrid::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentGrid::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentGrid::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentGrid::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
}

void* UIComponentGrid::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentGrid::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        );
}

const bool UIComponentGrid::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text,
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    std::vector<VectorInt4> child_window_offset_array;

    VectorInt2 layout_size;
    VectorInt2 desired_size;
    GetGridDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        child_window_offset_array
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    UIComponentDefault::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        _component_default.GetUVScroll(),
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        layout_size,
        desired_size,
        _component_default.GetLayout()
        );

    // Update geometry
    bool dirty = false;
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    in_out_node.UpdateTextureSize(texture_size, dirty);
    out_screen_space.Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    int trace = 0;
    auto& child_data_array = in_out_node.GetChildData();
    for (auto& child_data_ptr : child_data_array)
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;
        VectorInt4& child_window_offset = child_window_offset_array[trace];
        trace += 1;

        if (nullptr == child_data._component)
        {
            continue;
        }

        const VectorInt2 offset(child_window_offset.GetZ(), child_window_offset.GetW());
        const VectorInt2 window(child_window_offset.GetX(), child_window_offset.GetY());

        if (true == child_data._component->UpdateSize(
            in_draw_system,
            texture_size,
            offset,
            window,
            in_ui_scale,
            in_time_delta,
            *(child_data._geometry.get()),
            *(child_data._node.get()),
            out_screen_space,
            *(child_data._screen_space.get())
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

void UIComponentGrid::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    std::vector<VectorInt4> child_window_offset; // left to right, top to bottom

    GetGridDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        child_window_offset
        );
    return;
}

const bool UIComponentGrid::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    )
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

void UIComponentGrid::GetGridDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode&,// in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    std::vector<VectorInt4>& out_child_window_offset // left to right, top to bottom
    )
{
    out_layout_size = _component_default.GetInUseLayout().GetSize(in_parent_window, in_ui_scale);

    std::vector<int> horizontal_sizes;
    {
        float bias_sum = 0.0f;
        int total_reserved = 0;
        for (const auto& item : _horizontal_size_array)
        {
            const int size = item.GetSize().Calculate(out_layout_size, in_ui_scale);
            horizontal_sizes.push_back(size);
            total_reserved += size;
            bias_sum += item.GetRemainderBias();
        }

        if ((0 == bias_sum) || (_component_default.GetLayout().GetShrinkWidth()))
        {
            out_desired_size[0] = total_reserved;
        }
        else
        {
            out_desired_size[0] = std::max(out_layout_size.GetX(), total_reserved);

            const int horizontal_remainder = out_layout_size.GetX() - total_reserved;
            if (0 < horizontal_remainder)
            {
                int running_total = 0;
                const float bias_mul = bias_sum ? 1.0f / bias_sum : 0.0f;
                for (int index = 0; index < static_cast<int>(_horizontal_size_array.size()); ++index)
                {
                    const float bias = _horizontal_size_array[index].GetRemainderBias() * bias_mul;
                    const int add = static_cast<int>(horizontal_remainder * bias);
                    horizontal_sizes[index] += add;
                    running_total += horizontal_sizes[index];
                }
                horizontal_sizes.back() += (out_layout_size.GetX() - running_total);
            }
        }
    }

    const int height = out_layout_size.GetY();
    std::vector<int> vertical_sizes;
    {
        float bias_sum = 0.0f;
        int total_reserved = 0;
        for (const auto& item : _vertical_size_array)
        {
            const int size = item.GetSize().Calculate(out_layout_size, in_ui_scale);
            vertical_sizes.push_back(size);
            total_reserved += size;
            bias_sum += item.GetRemainderBias();
        }

        if ((0 == bias_sum) || (_component_default.GetLayout().GetShrinkHeight()))
        {
            out_desired_size[1] = total_reserved;
        }
        else
        {
            out_desired_size[1] = std::max(out_layout_size.GetY(), total_reserved);

            const int vertical_remainder = height - total_reserved;
            if (0 < vertical_remainder)
            {
                int running_total = 0;
                const float bias_mul = bias_sum ? 1.0f / bias_sum : 0.0f;
                for (int index = 0; index < static_cast<int>(_vertical_size_array.size()); ++index)
                {
                    const float bias = _vertical_size_array[index].GetRemainderBias() * bias_mul;
                    const int add = static_cast<int>(vertical_remainder * bias);
                    vertical_sizes[index] += add;
                    running_total += vertical_sizes[index];
                }
                vertical_sizes.back() += (height - running_total);
            }
        }
    }

    int sum_y = 0;
    for (int index_y = 0; index_y < static_cast<int>(vertical_sizes.size()); ++index_y)
    {
        int sum_x = 0;
        const int size_y = vertical_sizes[index_y];
        sum_y += size_y;
        for (int index_x = 0; index_x < static_cast<int>(horizontal_sizes.size()); ++index_x)
        {
            const int size_x = horizontal_sizes[index_x];
            out_child_window_offset.push_back(VectorInt4(
                size_x,
                size_y,
                sum_x,
                height - sum_y
                ));
            sum_x += size_x;
        }
    }

    out_layout_size = _component_default.GetInUseLayout().CalculateShrinkSize(out_layout_size, out_desired_size);

    return;
}
