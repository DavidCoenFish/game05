#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/locale/locale_enum.h"
#include "common/text/text_enum.h"

class LocaleSystem;

class UIDataString : public UIData
{
    typedef UIData TSuper;
public:
    /// let the text manager what locale hint to render text as, for harfbuz
    UIDataString(
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null = nullptr,
        const std::string& in_data = std::string(),
        const LocaleISO_639_1 in_locale = LocaleISO_639_1::Default,
        bool _width_limit_enabled = false,
        TextEnum::HorizontalLineAlignment _horizontal = TextEnum::HorizontalLineAlignment::Left,
        TextEnum::VerticalBlockAlignment _vertical = TextEnum::VerticalBlockAlignment::Bottom
        );
    virtual ~UIDataString();

    // Todo: change to visitor to future proof threading?
    const std::string& GetStringRef() const { return _data; }
    const LocaleISO_639_1 GetLocale() const { return _locale; }
    void SetString(const std::string& in_data);

private:
    /// Make component type match what the data wants, default is UIComponentCanvas
    virtual void UpdateHierarchy(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const int in_index = 0
        ) override;

    virtual void UpdateLayoutRender(
        IUIComponent& in_component,
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const UIScreenSpace& in_parent_screen_space
        ) override;

    /// GetChild desired size? layout is now part of UIData?
    virtual const VectorInt2 GetContentSize(
        IUIComponent& in_component,
        const VectorInt2& in_target_size, 
        const float in_ui_scale,
        UIHierarchyNodeChildData& in_component_owner
        ) override;

    virtual const VectorInt2 GetDesiredSize(
        IUIComponent& in_component,
        const VectorInt2& in_target_size, 
        const float in_ui_scale,
        UIHierarchyNodeChildData& in_component_owner
        ) override;

private:
    std::string _data;
    LocaleISO_639_1 _locale;
    bool _width_limit_enabled;
    TextEnum::HorizontalLineAlignment _horizontal;
    TextEnum::VerticalBlockAlignment _vertical;

};
