#pragma once

enum class UIOrientation
{
    TVertical, // TopToBottom
    THorizontal, // LeftRight
    //TGridLeftRightTopBottom?
    //TGridTopBottomLeftRight?
    //TFlowLeftRightTopBottom
    //TFlowTopBottomLeftRight
};

enum class UIEffectEnum
{
    TNone = 0, // this seemed like a good idea to have None be the default block shader, but constant buffer is different
    TDebug,
    TDropShadow,
    TInnerShadow,
    TRoundCorners,
    TGloss,
    TFill,
    TDesaturate,

    // TInnerShadow, // Invert of drop shadow, draw over solid, not transparent
    // TEdgeShadow,
    // TTextureMixRGBAndAlpha, // Use child 0 as rgb, and child 1 as alpha ~ take average of rgb to use as alpha
    // TBlend is not needed, the default shader will do a premultiplied alpha blend of the children
    // TChromeFill move fill to a different element?

    TCount
};

enum class UIStateFlag
{
    TNone = 0,
    /// Mouse cursor over item
    THover = 1 << 0, 
    /// Mouse click down / screen touch
    TTouch = 1 << 1, 
    /// List box selection/ navigation focus?
    TSelected = 1 << 2, 

    /// Disabled is expressed

    /// Mask for input pass to filter state flag, ie, the values set by the ui manager input pass
    TMaskInput = THover | TTouch, 

    /// Count of how many tint permutations
    TTintPermutationCount = 1 << 3,
    TTintMask = THover | TTouch | TSelected

};
