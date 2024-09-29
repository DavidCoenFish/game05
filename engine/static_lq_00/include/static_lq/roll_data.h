#pragma once

namespace static_lq
{
    struct RollData
    {
    public:
        /// the "W" of W + 2d10, example: 12+2d10, constant = 12
        int32_t _constant; 
        /// the "X" of "X"d"Y", example: 2d10, dice_count == 2
        int32_t _dice_count; 
        /// the "Y" of "X"d"Y" example: 2d10, dice_base == 10
        int32_t _dice_base; 
    };
}