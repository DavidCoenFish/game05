#include "common/common_pch.h"
#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_layout.h"

IUIComponent::IUIComponent()
{
    // Nop
}

IUIComponent::~IUIComponent()
{
    // Nop
}

const bool IUIComponent::SetLayout(const UILayout&)
{
    return false;
}

const UILayout& IUIComponent::GetLayout() const
{
    static const UILayout s_dummy;
    return s_dummy;
}

