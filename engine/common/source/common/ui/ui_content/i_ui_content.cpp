#include "common/common_pch.h"
#include "common/ui/ui_content/i_ui_content.h"

const int IUIContent::GetNewTypeID()
{
    static s_id = 0;
    s_id += 1;
    return s_id;
}

IUIContent::IUIContent()
{
    // Nop
}

