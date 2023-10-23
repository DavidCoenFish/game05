#pragma once

enum class UIEffectEnum
{
    TNone = 0, // this seemed like a good idea to have None be the default block shader, but constant buffer is different
    TDebug,
    TDropShadow,
    TRoundCorners,
    TGloss,
    // TInnerShadow, // Invert of drop shadow, draw over solid, not transparent
    // TEdgeShadow, // draw shadow against edge of texture
    // TDesaturate,
    // TDesaturateIfDisabled,
    // TTextureMixRGBAndAlpha, // Use child 0 as rgb, and child 1 as alpha ~ take average of rgb to use as alpha
    // TBlend is not needed, the default shader will do a premultiplied alpha blend of the children
    // TChromeFill move fill to a different element

    TCount
};
