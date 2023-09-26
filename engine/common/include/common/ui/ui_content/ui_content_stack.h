#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_default.h"

class VectorInt4;

enum class StackOrientation
{
    TVertical, // BottomTop
    THorizontal, // LeftRight
    //TGridLeftRightTopBottom?
    //TGridTopBottomLeftRight?
    //TFlowLeftRightTopBottom
    //TFlowTopBottomLeftRight
};

class UIContentStack : public IUIContent
{
public:
    UIContentStack(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout,
        const StackOrientation in_orientation,
        const UICoord& in_gap
        );
    virtual ~UIContentStack();

    const bool Set(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout,
        const StackOrientation in_orientation,
        const UICoord& in_gap
        );

private:
    //virtual const bool GetClearBackground(
    //    VectorFloat4& out_clear_colour
    //    ) const override;

    // Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool SetLayout(const UILayout& in_layout) override;

    virtual const bool UpdateHierarchy(
        //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    virtual const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    const VectorInt2 GetStackDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        std::vector<VectorInt4>& out_child_desired_size_offset
        );
private:
    UIContentDefault _content_default;
    StackOrientation _orientation;
    UICoord _gap;

};
