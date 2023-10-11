#include "common/common_pch.h"
#include "common/ui/ui_component/i_ui_component.h"

IUIComponent::IUIComponent()
{
    // Nop
}

IUIComponent::~IUIComponent()
{
    // Nop
}

//const bool IUIComponent::GetNeedsPreDraw() const
//{
//    return false;
//}

void IUIComponent::PreDraw(
    const UIManagerDrawParam&
    )
{
}


