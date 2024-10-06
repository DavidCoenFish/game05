#pragma once

#include "common/text/text_enum.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class TextBlock;
class TextFontImplementation;
class TextLocale;
class TextPreVertex;
class TextTexture;
class VectorInt2;
class VectorFloat2;
typedef struct FT_LibraryRec_* FT_Library;

class TextFont
{
public:
	TextFont(
		FT_Library in_library,
		const std::filesystem::path& in_font_file_path,
		TextTexture* const in_text_texture
		);
	~TextFont();

	void BuildPreVertexData(
		TextPreVertex& in_out_text_pre_vertex,
		VectorInt2& in_out_cursor, // allow multiple fonts to append pre vertex data
		const std::string& in_string_utf8,
		const TextLocale* const in_locale_token,
		const int in_font_size,
		const bool in_width_limit_enabled = false,
		const int in_width_limit = 0,
		const int in_new_line_height = 0,
		const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
		);

	/// Draw a glyph to a pixel blob, made to match the target size
	void DrawToPixels(
		std::vector<uint8_t>& out_data,
		const int in_target_width,
		const int in_target_height,
		const std::string& in_string_utf8,
		const TextLocale* const in_locale_token,
		const int in_font_size,
		const int in_offset_x = 0,
		const int in_offset_y = 0
		);

	void RestGlyphUsage();

private:
	std::unique_ptr<TextFontImplementation> _implementation;

};
