#pragma once

#include "common/math/vector_float4.h"

/// A ui elements render targets clear param, the background colour. transparent by default
class UIBaseColour
{
public:
	/// Default is transparent clear
	static const UIBaseColour FactoryDefault();
	/// Root node needs usually renders to backbuffer
	static const UIBaseColour FactoryRoot(const bool in_always_dirty);

	static const UIBaseColour FactoryGreyBackground();
	static const UIBaseColour FactoryRedBackground();
	static const UIBaseColour FactoryGreenBackground();
	static const UIBaseColour FactoryBlueBackground();

	/// Always dirty is usefully if the destination texture target, like the backbuffer, is draw to by other systems
	explicit UIBaseColour(
		const VectorFloat4& in_clear_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
		const bool in_clear_background = true,
		const bool in_draw_to_texture = true,
		const bool in_always_dirty = false
		);

	const VectorFloat4& GetClearColourRef() const { return _clear_colour; }
	const bool GetClearBackground() const { return _clear_background; }

	const bool GetDrawToTexture() const { return _draw_to_texture; }
	const bool GetAlwaysDirty() const { return _always_dirty; }

	const bool operator==(const UIBaseColour& in_rhs) const;
	const bool operator!=(const UIBaseColour& in_rhs) const;

private:
	VectorFloat4 _clear_colour;
	bool _clear_background;
	bool _draw_to_texture;
	bool _always_dirty;

};

