#include "common/common_pch.h"
#include "common/math/vector_float4.h"
#include "common/text/i_text_run_data.h"

class TextLocale;
class TextFont;
/// <summary>
/// 
/// </summary>
class TextRunDataString : public ITextRunData
{
public:
	explicit TextRunDataString(
		const std::string& in_string_utf8 = std::string(""),
		const TextLocale* const in_locale_token = nullptr,
		TextFont* const in_text_font = nullptr,
		const int in_font_size = 0,
		const float in_new_line_gap_ratio = 0.0f,
		const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
		);

private:
	virtual void BuildPreVertexData(
		TextPreVertex& in_out_pre_vertex_data,
		VectorInt2& in_out_cursor,
		const bool in_width_limit_enabled,
		const int in_width_limit,
		const float in_ui_scale
		) const override;

private:
	std::string _string_utf8;
	const TextLocale* const _locale_token;
	TextFont* const _text_font;
	int _font_size;
	float _new_line_gap_ratio;
	VectorFloat4 _colour;

};