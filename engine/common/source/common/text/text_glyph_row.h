#pragma once

// Rename TextGlyphRow
class TextGlyphRow
{
public:
	TextGlyphRow(
		const int in_mask_index,
		const int in_height,
		const int in_texture_pos_y
		);

	const int GetMaskIndex() const { return _mask_index; }
	const int GetHeight() const { return _height; }
	const int GetTexturePosY() const { return _texture_pos_y; }
	const int GetTextureHighestX() const { return _texture_highest_x; }
	void IncrementTextureHighestX(const int in_add_x);

private:
	int _mask_index;
	int _height;
	int _texture_pos_y;
	int _texture_highest_x;

};
