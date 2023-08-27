#include "common/common_pch.h"
#include "common/text/text_font.h"

#include "common/draw_system/draw_system.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/text/text_block.h"
#include "common/text/text_cell.h"
#include "common/text/text_locale.h"
#include "common/text/text_pre_vertex.h"
#include "common/text/text_texture.h"
#include "common/util/utf8.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
// Was not in the include folder, is this safe to use?
#include <harf_buzz\Source\hb-ft.h>

class TextFontImplementation
{
    typedef std::map<uint32_t, std::shared_ptr<TextCell>> TMapGlyphCell;

public:
    TextFontImplementation(
        FT_Library in_library,
        const std::filesystem::path& in_font_file_path,
        TextTexture* const in_text_texture
        )
        : _text_texture(in_text_texture)
    {
        FT_Error error;
        auto path = Utf8::Utf16ToUtf8(in_font_file_path.c_str());
        error = FT_New_Face(in_library, path.c_str(), 0, &_face);

        //https://github.com/tangrams/harfbuzz-example/blob/master/src/freetypelib.cpp
        for (int i = 0; i < _face->num_charmaps; i++) 
        {
            if (((_face->charmaps[i]->platform_id == 0)
                && (_face->charmaps[i]->encoding_id == 3))
                || ((_face->charmaps[i]->platform_id == 3)
                    && (_face->charmaps[i]->encoding_id == 1))) 
            {
                FT_Set_Charmap(_face, _face->charmaps[i]);
            }
        }

        _harf_buzz_font = hb_ft_font_create(_face, NULL);
    }

    ~TextFontImplementation()
    {
        hb_font_destroy(_harf_buzz_font);
        FT_Done_Face(_face);
    }

    void CalculateTextBounds(
        TextPreVertex& text_pre_vertex,
        VectorFloat2& in_out_cursor, // allow multiple fonts to append pre vertex data
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const int in_width_limit_new_line_height
        )
    {
        auto map_glyph_cell = FindMapGlyphCell(in_font_size);
        SetScale(in_font_size);

        hb_buffer_t* const buffer = MakeBuffer(
            in_string_utf8,
            in_locale_token
            );

        ShapeText(
            text_pre_vertex,
            in_out_cursor,
            in_string_utf8,
            buffer,
            *map_glyph_cell,
            in_width_limit_enabled,
            in_width_limit,
            in_width_limit_new_line_height
        );
        hb_buffer_destroy(buffer);
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
        FT_Set_Char_Size(_face, 0, in_glyph_size * 64, 72, 72);
        return;
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
            in_slot->bitmap.rows,
            in_slot->bitmap_left,
            in_slot->bitmap_top
            );

        in_out_map_glyph_cell[in_codepoint] = cell;
        return cell.get();
    }

    // hb_buffer_destroy(buffer);
    hb_buffer_t* MakeBuffer(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        )
    {
        hb_buffer_t* buffer = hb_buffer_create();
        hb_buffer_add_utf8(buffer, in_string_utf8.c_str(), -1, 0, -1);
        //set script, language, direction
        if (nullptr != in_locale_token)
        {
            hb_buffer_set_direction(buffer, in_locale_token->GetDirection());
            hb_buffer_set_script(buffer, in_locale_token->GetScript());
            hb_buffer_set_language(buffer, in_locale_token->GetLanguage());
        }
        else
        {
            hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
            hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
            hb_buffer_set_language(buffer, hb_language_from_string("en", -1));
        }
        return buffer;
    }

    void ShapeText(
        TextPreVertex& in_out_text_pre_vertex,
        VectorFloat2& in_out_cursor, // allow multiple fonts to append pre vertex data
        const std::string& in_string_utf8,
        hb_buffer_t* in_buffer,
        TMapGlyphCell& in_out_map_glyph_cell,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const int in_width_limit_new_line_height
        )
    {    
        hb_shape(_harf_buzz_font, in_buffer, NULL, 0);

        unsigned int glyph_count = 0;
        hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(in_buffer, &glyph_count);
        hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(in_buffer, &glyph_count);

        in_out_text_pre_vertex.Reserve(glyph_count);

        // start a new line each time you hit this cluster for the width limit
        std::vector<unsigned int> width_line_clusert_index; 
        if (true == in_width_limit_enabled)
        {
            float cursor_x = in_out_cursor.GetX();
            // Work out what line each glpyh should be on
            unsigned int current_cluster = std::numeric_limits<unsigned int>::max();// -1;
            for (unsigned int i = 0; i < glyph_count; i++) 
            {
                hb_glyph_info_t& info = glyph_info[i];
                bool start_new_line = false;

                // Have at least one cluster per line
                if (std::numeric_limits<unsigned int>::max() == current_cluster)
                {
                    current_cluster = info.cluster;
                }
                else if (info.cluster != current_cluster)
                {
                    float trace_x = cursor_x;
                    for (unsigned int ahead = i + 0; ahead < glyph_count; ahead++) 
                    {
                        float x_advance = (float)glyph_pos[ahead].x_advance / 64.0f;
                        trace_x += x_advance;

                        // We want to break the line if cluster goes past in_width_limit, and we can break on the cluster
                        const hb_glyph_flags_t flag = hb_glyph_info_get_glyph_flags(&glyph_info[ahead]);
                        if ((in_width_limit < trace_x) &&
                            (0 == (flag & HB_GLYPH_FLAG_UNSAFE_TO_BREAK)))
                        {
                            width_line_clusert_index.push_back(glyph_info[ahead].cluster);
                            current_cluster = std::numeric_limits<unsigned int>::max();
                            i = ahead;
                            cursor_x = 0.0f;
                            start_new_line = true;
                            break;
                        }
                    }
                }

                if (false == start_new_line)
                {
                    // If we did not find a width limited new line, just break? no more work?
                    break;
                }
            }
        }
        // Now place each glyph
        int line_index = 0;

        unsigned int last_checked_cluster_index = 0;
        for (unsigned int i = 0; i < glyph_count; i++) 
        {
            // catch any new line characters from the input string, TODO: is cluster realy the byte offset of in_string_utf8, or the unicode char index
            const unsigned int current_cluster = glyph_info[i].cluster;
            if (in_string_utf8[current_cluster] == '\n')
            {
                in_out_text_pre_vertex.StartNewLine(in_out_cursor);
                continue;
            }

            // Start new line
            if ((line_index < (int)width_line_clusert_index.size()) &&
                (width_line_clusert_index[line_index] == current_cluster))
            {
                line_index += 1;
                in_out_text_pre_vertex.StartNewLine(in_out_cursor);
            }

            hb_codepoint_t codepoint = glyph_info[i].codepoint;

            float x_offset = (float)glyph_pos[i].x_offset / 64.0f;
            float y_offset = (float)glyph_pos[i].y_offset / 64.0f;
            float x_advance = (float)glyph_pos[i].x_advance / 64.0f;
            float y_advance = (float)glyph_pos[i].y_advance / 64.0f;

            FT_Error error = FT_Load_Glyph(_face, codepoint, FT_LOAD_DEFAULT);
            error;
            FT_GlyphSlot slot = _face->glyph;
            FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

            auto cell = FindOrMakeCell(codepoint, slot, in_out_map_glyph_cell);
            if (nullptr != cell)
            {
                in_out_text_pre_vertex.AddPreVertex(
                    cell, 
                    in_out_cursor[0] + x_offset, 
                    in_out_cursor[1] + y_offset,
                    (float)in_width_limit_new_line_height
                    );
            }
            in_out_cursor[0] += x_advance;
            in_out_cursor[1] += y_advance;
        }

        return;
    }

private:
    FT_Face _face;
    hb_font_t* _harf_buzz_font;

    TextTexture* _text_texture;

    // For each font size, have a glyph cell referencing the uv on _text_texture
    std::map<uint32_t, std::shared_ptr<TMapGlyphCell>> _map_size_glyph_cell;

};

TextFont::TextFont(
    FT_Library in_library,
    const std::filesystem::path& in_font_file_path,
    TextTexture* const in_text_texture
    )
{
    _implementation = std::make_unique<TextFontImplementation>(
        in_library,
        in_font_file_path,
        in_text_texture
        );
}

TextFont::~TextFont()
{
    _implementation.reset();
}

void TextFont::CalculateTextBounds(
    TextPreVertex& text_pre_vertex,
    VectorFloat2& in_out_cursor, // allow multiple fonts to append pre vertex data
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const int in_width_limit_new_line_height
    )
{
    _implementation->CalculateTextBounds(
        text_pre_vertex, 
        in_out_cursor,
        in_string_utf8,
        in_locale_token,
        in_font_size,
        in_width_limit_enabled,
        in_width_limit,
        in_width_limit_new_line_height
        );
    return;
}

void TextFont::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
