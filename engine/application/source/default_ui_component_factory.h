#pragma once

class UIManager;
struct UIDataTextRunStyle;

namespace DefaultUIComponentFactory
{
    void Populate(
        UIManager& in_ui_manager
        );

    const UIDataTextRunStyle* const GetDefaultTextStyle();
    const std::string GetTextRunTagSmallFont();

};
