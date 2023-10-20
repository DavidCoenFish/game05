#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_coord.h"

class VectorInt4;

/// Size data for the component Grid
class UIComponentGridSizeData
{
public: 
    explicit UIComponentGridSizeData(
        const UICoord& in_size = UICoord(),
        const float in_remainder_bias = 0.0f
        );

    const bool operator==(const UIComponentGridSizeData& in_rhs) const;
    const bool operator!=(const UIComponentGridSizeData& in_rhs) const;

    const UICoord& GetSize() const { return _size; }
    const float GetRemainderBias() const { return _remainder_bias; }

private:
    /// The minimal size of the parent to use
    UICoord _size;
    /// Our bias of how much of the remainder of a parent size t use
    float _remainder_bias;
};

/// put the childrent, left to right, top to bottom, into the cells defined by the arrays of UIComponentGridSizeData
class UIComponentGrid : public IUIComponent
{
public:
    UIComponentGrid(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        // shared pointers or raw? think raw
        const std::vector<UIComponentGridSizeData>& in_horizontal_size_array, // Left to right
        const std::vector<UIComponentGridSizeData>& in_vertical_size_array // Top to bottom
        );
    virtual ~UIComponentGrid();

    const bool Set(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::vector<UIComponentGridSizeData>& in_horizontal_size_array, // Left to right
        const std::vector<UIComponentGridSizeData>& in_vertical_size_array // Top to bottom
        );

private:
    /// Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    /// Make sorting children easier
    virtual void* GetSourceToken() const override;

    /// make the hirearchy match the model (UIData)
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    /// convert the layout data and parent size to the texture size, geometry size and uv
    /// certain component types may do slightly different operations, but the default is to call GetDesiredSize
    virtual void UpdateSize(
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
        ) override;

    /// certain layout data allows shrink, and certain componets may have different logic, such as stack component
    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    /// deal with the component being drawn to the node texture
    virtual const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

    /// we don't expand for children, but do have to get the offest and window size for each child cells
    void GetGridDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        std::vector<VectorInt4>& out_child_window_offset // left to right, top to bottom
        );


private:
    /// Default properties of a component
    UIComponentDefault _content_default;
    /// Left to right
    std::vector<UIComponentGridSizeData> _horizontal_size_array;
    /// Top to bottom
    std::vector<UIComponentGridSizeData> _vertical_size_array;

};
