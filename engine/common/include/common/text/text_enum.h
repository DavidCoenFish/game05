#pragma once

namespace TextEnum
{
    //enum WidthTreatment
    //{
    //    None = 0, // don't 
    //    HardLimitWidth
    //};
    enum class HorizontalLineAlignment
    {
        Left,
        Middle,
        Right
    };
    enum class VerticalBlockAlignment
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
}
