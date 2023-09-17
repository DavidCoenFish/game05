#include "common/common_pch.h"
#include "common/ui/ui_content/i_ui_content.h"

IUIContent::IUIContent()
{
    // Nop
}

IUIContent::~IUIContent()
{
    // Nop
}

const bool IUIContent::GetNeedsPreDraw() const
{
    return false;
}

void IUIContent::PreDraw(
    const UIManagerDrawParam&
    )
{
}


