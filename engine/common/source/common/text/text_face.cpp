#include "common/common_pch.h"
#include "common/text/text_face.h"

#include "common/draw_system/draw_system.h"
#include "common/file_system/file_system.h"
#include "common/math/vector_int2.h"
#include "common/text/text_block.h"
#include "common/text/text_cell.h"
#include "common/text/text_texture.h"
#include "common/util/utf8.h"
#include "common/util/vector_helper.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
// Was not in the include folder, is this safe to use?
#include <harf_buzz\Source\hb-ft.h>

class TextFaceImplementation
{
    typedef std::map<uint32_t, std::shared_ptr<TextCell>> TMapGlyphCell;

public:
    TextFaceImplementation(
        FT_Library in_library,
        const std::filesystem::path& in_font_file_path,
        TextTexture* const in_text_texture
        )
        : _text_texture(in_text_texture)
    {
        FT_Error error;
        auto path = Utf8::Utf16ToUtf8(in_font_file_path.c_str());
        error = FT_New_Face(in_library, path.c_str(), 0, &_face);

        //hb_blob_t* harf_buzz_blob = hb_blob_create_from_file(path.c_str());
        //_harf_buzz_face = hb_face_create(harf_buzz_blob, 0);
        _harf_buzz_face = hb_ft_face_create_referenced(_face);
        //hb_blob_destroy(harf_buzz_blob);
        //unsigned int upem = hb_face_get_upem(_harf_buzz_face);

        //_harf_buzz_font = hb_font_create(_harf_buzz_face);
        _harf_buzz_font = hb_ft_font_create_referenced(_face);
        //hb_font_set_scale(_harf_buzz_font, upem, upem);

        //hb_ft_font_set_funcs(_harf_buzz_font);
    }

    ~TextFaceImplementation()
    {
        hb_font_destroy(_harf_buzz_font);
        //hb_face_destroy(_harf_buzz_face);
        FT_Done_Face(_face);
    }

    std::shared_ptr<TextBlock> MakeBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const int in_glyph_size,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Enum::Left,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Enum::Top
        )
    {
        auto map_glyph_cell = FindMapGlyphCell(in_glyph_size);
        SetScale(in_glyph_size);

        std::vector<uint8_t> vertex_raw_data;
        ShapeText(
            *map_glyph_cell,
            in_string_utf8,
            vertex_raw_data,
            in_containter_size
            );

        auto result = std::make_shared<TextBlock>(
            in_draw_system,
            in_command_list,
            in_string_utf8,
            in_containter_size,
            in_width_limit_enabled,
            in_width_limit,
            in_horizontal_line_alignment,
            in_vertical_block_alignment,
            vertex_raw_data
            );

        return result;
    }

    VectorInt2 CalculateTextBounds(
        const std::string& in_string_utf8,
        const int in_glyph_size,
        const bool in_width_limit_enabled,
        const int in_width_limit
        )
    {
        auto map_glyph_cell = FindMapGlyphCell(in_glyph_size);
        SetScale(in_glyph_size);

        // TODO
        return VectorInt2();
    }

    void RestGlyphUsage()
    {
        _map_size_glyph_cell.clear();
        return;
    }

private:
    TMapGlyphCell* const FindMapGlyphCell(const int in_glyph_size)
    {
        TMapGlyphCell* map_glyph_cell = nullptr;
        auto found = _map_size_glyph_cell.find(in_glyph_size);
        if (found == _map_size_glyph_cell.end())
        {
            auto temp = std::make_shared<TMapGlyphCell>();
            _map_size_glyph_cell[in_glyph_size] = temp;
            map_glyph_cell = temp.get();
        }
        else
        {
            map_glyph_cell = found->second.get();
        }

        return map_glyph_cell;
    }

    void SetScale(const int in_glyph_size)
    {
#if 0
        FT_Error error;
        error = FT_Set_Char_Size(_face, in_glyph_size, in_glyph_size, 72, 72);
        hb_font_set_scale(_harf_buzz_font, in_glyph_size, in_glyph_size);
        hb_font_set_ptem(_harf_buzz_font, 72);
#else
        FT_Error error;
        error = FT_Set_Char_Size(_face, in_glyph_size, in_glyph_size, 2048, 2048);
        hb_font_set_scale(_harf_buzz_font, in_glyph_size, in_glyph_size);
#endif
    }

    TextCell* FindOrMakeCell(
        hb_codepoint_t in_codepoint,
        FT_GlyphSlot in_slot,
        TMapGlyphCell& in_out_map_glyph_cell
        )
    {
        auto found = in_out_map_glyph_cell.find(in_codepoint);
        if (found != in_out_map_glyph_cell.end())
        {
            return found->second.get();
        }

        auto cell = _text_texture->MakeCell(
            in_slot->bitmap.buffer,
            in_slot->bitmap.width,
            in_slot->bitmap.rows
            );

        in_out_map_glyph_cell[in_codepoint] = cell;
        return cell.get();
    }

    void AddVertexData(
        std::vector<uint8_t>& out_vertex_raw_data,
        TextCell* in_cell,
        const hb_position_t in_pos_x,
        const hb_position_t in_pos_y,
        const VectorInt2& in_containter_size
        )
    {
        const VectorInt2 width_height = in_cell->GetWidthHeight();
        const VectorFloat4 uv = in_cell->GetUV();
        const VectorFloat4 mask = in_cell->GetMask();

        const VectorFloat4 pos = VectorFloat4(
            (((float)in_pos_x / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
            (((float)in_pos_y / (float)in_containter_size.GetY()) * 2.0f) - 1.0f,
            (((float)(in_pos_x + width_height.GetX()) / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
            (((float)(in_pos_y + width_height.GetY()) / (float)in_containter_size.GetY()) * 2.0f) - 1.0f
            );

        //float2 _position : Position;
        //float2 _uv : TEXCOORD0;
        //float4 _mask : COLOR0;

        //0.0f, 0.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

        //0.0f, 1.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

        //1.0f, 0.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

        //1.0f, 0.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[1]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

        //0.0f, 1.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[0]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

        //1.0f, 1.0f,
        VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[2]);
        VectorHelper::AppendValue(out_vertex_raw_data, uv[3]);
        VectorHelper::AppendValue(out_vertex_raw_data, mask);

    }

    void ShapeText(
        TMapGlyphCell& in_out_map_glyph_cell,
        const std::string& in_string_utf8,
        std::vector<uint8_t>& out_vertex_raw_data,
        const VectorInt2& in_containter_size
        )
    {    
        hb_buffer_t* buffer = hb_buffer_create();
        hb_buffer_add_utf8(buffer, in_string_utf8.c_str(), -1, 0, -1);

        //set script, language, direction
        hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
        hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
        hb_buffer_set_language(buffer, hb_language_from_string("en", -1));

        unsigned int glyph_count = 0;

        hb_shape(_harf_buzz_font, buffer, NULL, 0);

        hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
        hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buffer, &glyph_count);

        hb_position_t cursor_x = 0;
        hb_position_t cursor_y = 0;
        for (unsigned int i = 0; i < glyph_count; i++) 
        {
            hb_codepoint_t codepoint = glyph_info[i].codepoint;
            hb_position_t x_offset = glyph_pos[i].x_offset;
            hb_position_t y_offset = glyph_pos[i].y_offset;
            hb_position_t x_advance = glyph_pos[i].x_advance;
            hb_position_t y_advance = glyph_pos[i].y_advance;
            // draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
            FT_Error error = FT_Load_Char(_face, codepoint, FT_LOAD_RENDER);
            FT_GlyphSlot slot = _face->glyph;

            auto cell = FindOrMakeCell(codepoint, slot, in_out_map_glyph_cell);
            AddVertexData(out_vertex_raw_data, cell, cursor_x + x_offset, cursor_y + y_offset, in_containter_size);

            cursor_x += x_advance / 72;
            cursor_y += y_advance;
        }

        hb_buffer_destroy(buffer);
    }

private:
    FT_Face _face;
    hb_face_t* _harf_buzz_face;
    hb_font_t* _harf_buzz_font;
    TextTexture* _text_texture;
    //unsigned int _upem;

    std::map<uint32_t, std::shared_ptr<TMapGlyphCell>> _map_size_glyph_cell;

};

TextFace::TextFace(
    FT_Library in_library,
    const std::filesystem::path& in_font_file_path,
    TextTexture* const in_text_texture
    )
{
    _implementation = std::make_unique<TextFaceImplementation>(
        in_library,
        in_font_file_path,
        in_text_texture
        );
}

TextFace::~TextFace()
{
    _implementation.reset();
}

std::shared_ptr<TextBlock> TextFace::MakeBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::string& in_string_utf8,
    const int in_char_size,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
{
    return _implementation->MakeBlock(
        in_draw_system,
        in_command_list,
        in_string_utf8,
        in_char_size,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

VectorInt2 TextFace::CalculateTextBounds(
    const std::string& in_string_utf8,
    const int in_char_size,
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    return _implementation->CalculateTextBounds(
        in_string_utf8,
        in_char_size,
        in_width_limit_enabled,
        in_width_limit
        );
}

void TextFace::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
