#pragma once

#include "common/text/text_enum.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class IconFont;
class ITextRunData;
class Shader;
class TextFont;
class TextLocale;
class TextRunImplementation;
class TextTexture;
class VectorInt2;

class TextRun
{
public:
	static std::shared_ptr<ITextRunData> MakeTextRunDataString(
		const std::string& in_string_utf8 = std::string(""),
		TextLocale* const in_locale_token = nullptr,
		TextFont* const in_text_font = nullptr,
		const int in_font_size = 0,
		const float in_new_line_gap_ratio = 0.0f,
		const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	static std::shared_ptr<ITextRunData> MakeTextRunDataIcon(
		const int in_icon_id = 0,
		IconFont* const in_icon_font = nullptr,
		const VectorFloat4& in_colour_tint = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
		const float in_new_line_gap_ratio = 0.0f
		);

	// Do we keep the defaults, and each run has the settings? or simplify so we just have global settings at top, and data for each run
	TextRun(
		const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
		const VectorInt2& in_container_size,
		const bool in_width_limit_enabled = false,
		const int in_width_limit = 0,
		const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Left,
		const TextEnum::VerticalBlockAlignment in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Top,
		const int in_em_size = 0,
		const float in_ui_scale = 1.0f
		);

	~TextRun();

	// Get the natural size required by the text using current width limit if enabled
	VectorInt2 GetTextBounds();

	const bool SetTextRunArray(
		const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array
		);
	const bool SetTextContainerSize(
		const VectorInt2& in_size
		);
	const bool SetWidthLimit(
		const bool in_width_limit_enabled,
		const int in_width_limit
		);
	const bool SetWidthLimitEnabled(
		const bool in_width_limit_enabled
		);
	const bool SetWidthLimitWidth(
		const int in_width_limit
		);
	const bool SetHorizontalLineAlignment(
		const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
		);
	const bool SetVerticalBlockAlignment(
		const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
		);
	const bool SetEMSize(
		const int in_em_size
		);
	const bool SetUIScale(
		const float in_ui_scale
		);

	const bool Set(
		const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
		const VectorInt2& in_size,
		const bool in_width_limit_enabled,
		const int in_width_limit,
		const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
		const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
		const int in_em_size,
		const float in_ui_scale
		);

	// Rather than a seperate Update function, that could be forgoten, have the update in the getter, results in out_geometry_dirty

	//GeometryGeneric* const GetGeometryRef(
	//std::shared_ptr<GeometryGeneric>& GetGeometryRef(
	//	bool& out_geometry_dirty,
	//	DrawSystem* const in_draw_system,
	//	ID3D12GraphicsCommandList* const in_command_list
	//	);
	void Draw(
		DrawSystem* const in_draw_system,
		DrawSystemFrame* const in_draw_system_frame,
		std::shared_ptr<Shader>& in_shader,
		TextTexture& in_text_texture
		);

private:
	std::unique_ptr<TextRunImplementation> _implementation;

};
