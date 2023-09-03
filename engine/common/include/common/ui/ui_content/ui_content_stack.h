#pragma once

#include "common/ui/ui_content/ui_content_default.h"

class UIContentStack : public UIContentDefault
{
public:
    UIContentStack();
    virtual ~UIContentStack();

private:
    virtual const int GetClassTypeID() const override;

};
