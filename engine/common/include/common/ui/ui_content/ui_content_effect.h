#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_default.h"

enum class UIEffectEnum;

class UIContentEffect : public IUIContent
{
public:
    UIContentEffect(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d
        );
    ~UIContentEffect();

    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout
        );

    // return true if modified, else false
    const bool Set(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d
        );

private:
    // Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool SetLayout(const UILayout& in_layout) override;

    virtual const bool UpdateHierarchy(
        UIData* const in_data,
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
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        ) override;

    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    //virtual const bool GetNeedsPreDraw() const override;
    virtual void PreDraw(
        const UIManagerDrawParam& in_param
        ) override;

private:
    UIContentDefault _content_default;

    UIEffectEnum _type;
    UICoord _coord_a;
    UICoord _coord_b;
    UICoord _coord_c;
    UICoord _coord_d;

    std::shared_ptr<Shader> _shader;
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};
