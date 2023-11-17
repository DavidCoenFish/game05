#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/text/text_enum.h"

class TextBlock;
class TextFont;
class TextLocale;

class UIComponentString : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentString(
        std::unique_ptr<TextBlock>& in_text_block
        );
    ~UIComponentString();

    const bool SetText(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        );

    // return true if modified, else false
    const bool Set(
        TextFont& in_font, 
        const int in_font_size,
        const float in_new_line_gap_ratio,
        const bool in_width_limit_enabled,
        const TextEnum::HorizontalLineAlignment in_horizontal, 
        const TextEnum::VerticalBlockAlignment in_vertical,
        const VectorFloat4& in_text_colour
        );

    const VectorInt2 GetDesiredSize(
        const VectorInt2& in_layout_size,
        const float in_ui_scale
        );

    void SetContainerSize(const VectorInt2& in_size);

private:
    //virtual const bool UpdateHierarchy(
    //    UIData* const in_data,
    //    UIHierarchyNodeChildData& in_out_child_data,
    //    const UIHierarchyNodeUpdateHierarchyParam& in_param
    //    ) override;

    //virtual const bool UpdateSize(
    //    DrawSystem* const in_draw_system,
    //    const VectorInt2& in_parent_size,
    //    const VectorInt2& in_parent_offset,
    //    const VectorInt2& in_parent_window,
    //    const float in_ui_scale,
    //    const float in_time_delta, 
    //    UIGeometry& in_out_geometry, 
    //    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    //    const UIScreenSpace& in_parent_screen_space,
    //    UIScreenSpace& out_screen_space
    //    ) override;

    //virtual void GetDesiredSize(
    //    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    //    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    //    const VectorInt2& in_parent_window,
    //    const float in_ui_scale,
    //    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    //    ) override;

    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

private:
    //bool _use_parent_size_for_width_limit;
    std::unique_ptr<TextBlock> _text_block;

};
