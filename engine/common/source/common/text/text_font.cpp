#include "common/common_pch.h"
#include "common/text/text_font.h"

#include "common/draw_system/draw_system.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/dsc_math.h"
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

    void BuildPreVertexData(
        TextPreVertex& text_pre_vertex,
        VectorInt2& in_out_cursor, // allow multiple fonts to append pre vertex data
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const int in_new_line_height,
        const VectorFloat4& in_colour
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
            in_new_line_height,
            in_colour
        );
        hb_buffer_destroy(buffer);
    }

    void DrawToPixels(
        std::vector<uint8_t>& out_data,
        const int in_target_width,
        const int in_target_height,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size
        )
    {
        out_data.resize(in_target_width * in_target_height * 4, 0);

        SetScale(in_font_size);
        hb_buffer_t* const buffer = MakeBuffer(
            in_string_utf8,
            in_locale_token
            );

        hb_shape(_harf_buzz_font, buffer, NULL, 0);

        unsigned int glyph_count = 0;
        hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
        //hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buffer, &glyph_count);

        if (0 < glyph_count)
        {
            hb_codepoint_t codepoint = glyph_info[0].codepoint;

            FT_Error error = FT_Load_Glyph(_face, codepoint, FT_LOAD_DEFAULT);
            error;

            FT_GlyphSlot slot = _face->glyph;
            FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

            for (uint32_t y = 0; y < slot->bitmap.rows; ++y)
            {
                for (uint32_t x = 0; x < slot->bitmap.width; ++x)
                {
                    //const int dest_data_index = ((((found_row->GetTexturePosY() + y) * _texture_dimention) + (found_row->GetTextureHighestX() + x)) * 4) + found_row->GetMaskIndex();
                    const int dest_data_index = ((y * in_target_width) + x) * 4;
                    const int buffer_index = ((y * slot->bitmap.width) + x);
                    out_data[dest_data_index + 3] = slot->bitmap.buffer[buffer_index];
                }
            }
        }

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
        //FT_Set_Char_Size(_face, in_glyph_size * 64, in_glyph_size * 64, 72, 72);
        FT_Set_Pixel_Sizes(_face, in_glyph_size, in_glyph_size);
        return;
    }

    TextCell* FindCell(
        hb_codepoint_t in_codepoint,
        TMapGlyphCell& in_out_map_glyph_cell
        )
    {
        auto found = in_out_map_glyph_cell.find(in_codepoint);
        if (found != in_out_map_glyph_cell.end())
        {
            return found->second.get();
        }
        return nullptr;
    }

    TextCell* MakeCell(
        hb_codepoint_t in_codepoint,
        FT_GlyphSlot in_slot,
        TMapGlyphCell& in_out_map_glyph_cell
        )
    {
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
        VectorInt2& in_out_cursor, // allow multiple fonts to append pre vertex data
        const std::string& in_string_utf8,
        hb_buffer_t* in_buffer,
        TMapGlyphCell& in_out_map_glyph_cell,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const int in_width_limit_new_line_height,
        const VectorFloat4& in_colour
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
            int cursor_x = in_out_cursor.GetX();
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
                    int trace_x = cursor_x;
                    for (unsigned int ahead = i + 0; ahead < glyph_count; ahead++) 
                    {
                        const int x_advance = glyph_pos[ahead].x_advance / 64;
                        trace_x += x_advance;

                        // We want to break the line if cluster goes past in_width_limit, and we can break on the cluster
                        const hb_glyph_flags_t flag = hb_glyph_info_get_glyph_flags(&glyph_info[ahead]);
                        if ((in_width_limit < trace_x) &&
                            (0 == (flag & HB_GLYPH_FLAG_UNSAFE_TO_BREAK)))
                        {
                            // we don't set current_cluster to max (flag empty) as the new line is not empty, it will start with our "ahead" cluster
                            width_line_clusert_index.push_back(glyph_info[ahead].cluster);
                            i = ahead;
                            cursor_x = x_advance;
                            start_new_line = true;
                            break;
                        }
                    }
                    if (false == start_new_line)
                    {
                        // If we did not find a width limited new line, just break? no more work?
                        break;
                    }
                }

                if (false == start_new_line)
                {
                    const int x_advance = glyph_pos[i].x_advance / 64;
                    cursor_x += x_advance;
                }
            }
        }
        // Now place each glyph
        int line_index = 0;
        const int colour = DscMath::ConvertColourToInt(in_colour);
        for (unsigned int i = 0; i < glyph_count; i++) 
        {
            // catch any new line characters from the input string
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

            const int x_offset = glyph_pos[i].x_offset / 64;
            const int y_offset = glyph_pos[i].y_offset / 64;
            //float x_advance = (float)glyph_pos[i].x_advance / 64.0f;
            //float y_advance = (float)glyph_pos[i].y_advance / 64.0f;

            FT_Error error = FT_Load_Glyph(_face, codepoint, FT_LOAD_DEFAULT);
            error;

            FT_GlyphSlot slot = _face->glyph;
            auto cell = FindCell(codepoint, in_out_map_glyph_cell);
            if (nullptr == cell)
            {
                // only ask to render the glyph to the slot->bitmap if we didn't find the cell, ie, don't have cell to re-use
                FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

                cell = MakeCell(codepoint, slot, in_out_map_glyph_cell);
            }

            if (nullptr != cell)
            {
                in_out_text_pre_vertex.AddPreVertex(
                    *cell, 
                    in_out_cursor[0] + x_offset, 
                    in_out_cursor[1] + y_offset,
                    in_width_limit_new_line_height,
                    colour
                    );
            }
            const int x_advance = slot->advance.x / 64;
            const int y_advance = slot->advance.y / 64;

            in_out_cursor[0] += x_advance;
            in_out_cursor[1] += y_advance;

            in_out_text_pre_vertex.AddCursor(
                in_out_cursor[0],
                in_out_cursor[1]
                );
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

void TextFont::BuildPreVertexData(
    TextPreVertex& in_out_text_pre_vertex,
    VectorInt2& in_out_cursor, // allow multiple fonts to append pre vertex data
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const int in_new_line_height,
    const VectorFloat4& in_colour
    )
{
    _implementation->BuildPreVertexData(
        in_out_text_pre_vertex, 
        in_out_cursor,
        in_string_utf8,
        in_locale_token,
        in_font_size,
        in_width_limit_enabled,
        in_width_limit,
        in_new_line_height,
        in_colour
        );
    return;
}

void TextFont::DrawToPixels(
    std::vector<uint8_t>& out_data,
    const int in_target_width,
    const int in_target_height,
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size
    )
{
    _implementation->DrawToPixels(
        out_data,
        in_target_width,
        in_target_height,
        in_string_utf8,
        in_locale_token,
        in_font_size
        );
    return;
}


void TextFont::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
