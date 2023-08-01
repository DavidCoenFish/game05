#pragma once

namespace TextEnum
{
    //enum WidthTreatment
    //{
    //    None = 0, // don't 
    //    HardLimitWidth
    //};
    struct HorizontalLineAlignment
    {
        enum Enum
        {
            Left,
            Middle,
            Right
        };
    };
    struct VerticalBlockAlignment
    {
        enum Enum
        {
            // Use text bounds
            Top,
            Middle, 
            Bottom,
            // Use glyph size
            MiddleEM, 
            TopEM,
            BottomEM,
        };
    };
}
