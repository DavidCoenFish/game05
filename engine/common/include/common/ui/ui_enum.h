#pragma once

enum class UIDataDirty
{
    TNone = 0,
    THierarchy = 1 << 0,
    TLayout = 1 << 1, // flag the related UIComponent as layout dirty
    // TAutoScroll
    TRender = 1 << 2, // flag the related UIComponent as render dirty

    TLayoutRender = TLayout | TRender
};

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
    TDropShadow, // data[offset x, offset y, radius, ]
    TInnerShadow,
    TRoundCorners, // data[Top right, top left, bottom left, bottom right]
    TRoundCornersHollow,
    TGloss,
    TFill, // use tint to make chrom gradient
    TDesaturate,
    //TTint, // just combine input with tint. this is the same was what the default shader does, but effect has _state_flag_tint_array. add _state_flag_tint_array to all?

    // TInnerShadow, // Invert of drop shadow, draw over solid, not transparent
    // TEdgeShadow,
    // TTextureMixRGBAndAlpha, // Use child 0 as rgb, and child 1 as alpha ~ take average of rgb to use as alpha
    // TBlend is not needed, the default shader will do a premultiplied alpha blend of the children
    // TChromeFill move fill to a different element?

    TCount
};

/// The effect shaders use an additional shader constant buffer to the default ui shader, complicating intermingling
/// possibly could put a switch in the UIComponentEffect code to support, or have seperate enums
enum class UIShaderEnum
{
    TDefault = 0,

    TEffectDebug,
    TEffectDropShadow,
    TEffectInnerShadow,
    TEffectRoundCorners,
    TEffectRoundCornersHollow,
    TEffectGloss,
    TEffectFill,
    TEffectDesaturate,
    //TEffectTint,

    TCount,
    TEffectStart = TEffectDebug
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

    /// Mask for input pass to filter state flag, ie, the values set by the ui manager input pass
    TMaskInput = THover | TTouch, 
    /// Count of how many tint permutations
    TTintPermutationCount = 1 << 3,
    /// Mask for tint types
    TTintMask = THover | TTouch | TSelected,

    /// Disabled is expressed by UICompnentDisable, but need a way to comunicate with input that node is disabled?
    TDisable = 1 << 3,
    /// Manual scroll wants to optionally hide scroll bars when child doesn't need to scroll. 
    /// Warning, kind of needs to be set by a parent, as if set by its self, the code to turn it back on may never be reached, ref: UIComponentDrift
    THidden = 1 << 4,

    /// disable the automatic ppinpong of uv for x axis
    TManualScrollX = 1 << 5,
    /// disable the automatic ppinpong of uv for y axis
    TManualScrollY = 1 << 6,

    /// A layout has changed
    TLayoutDirty = 1 << 29,
    TResourceDirty = 1 << 30, // also if effect param change

    TRenderDirty = 1 << 31

};

/// will these need to be flags, allow "shift control right click"? start with simple
enum class UITouchFlavour
{
    TNone,
    TMouseLeft,
    TMouseRight,
};

/// should this just be changed to directly using windows virtual key id? or vaugely allow remapping
/// This was intended for ui navigation control, but what if we want general key presses to be comunicated with some component
enum class UINavigationType
{
    TNone,
    TLeft, // keys have amount 1?
    TRight,
    TUp,
    TDown,
    TScroll, //mouse scroll has an ammount? or just add an amount to everything, allows dpad?
    TDpadHorizontal,
    TDpadVertical
};

enum class UISlideDirection
{
    TNone,
    TFromLeft, //start with node out to the left, and drift in to the default pos 
    TFromTop,
    TFromRight,
    TFromBottom

};

enum class UITooltipType
{
    TNone,
    TRelativeToTouch,
    TBottomLeft,
};

