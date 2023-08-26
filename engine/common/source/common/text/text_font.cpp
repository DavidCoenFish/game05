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
#include "common/text/text_texture.h"
#include "common/util/utf8.h"
#include "common/util/vector_helper.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
// Was not in the include folder, is this safe to use?
#include <harf_buzz\Source\hb-ft.h>

namespace
{
    struct PreVertexData
    {
        VectorFloat4 _pos_low_high;
        VectorFloat4 _uv_low_high;
        VectorFloat4 _mask;
        int _line_index;
    };
}

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

    VectorInt2 CalculateTextBounds(
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

        std::vector<PreVertexData> pre_vertex_data;
        VectorFloat2 vertical_bounds(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::min()
            );
        std::vector<VectorFloat2> horizontal_bounds;
        horizontal_bounds.push_back(VectorFloat2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min()));
        ShapeText(
            &pre_vertex_data,
            vertical_bounds,
            horizontal_bounds,
            buffer,
            *map_glyph_cell,
            in_width_limit_enabled,
            in_width_limit,
            in_width_limit_new_line_height
        );
        hb_buffer_destroy(buffer);

        VectorFloat2 horizontal(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::min()
            );
        for (const auto& iter : horizontal_bounds)
        {
            horizontal[0] = std::min(horizontal[0], iter[0]);
            horizontal[1] = std::max(horizontal[1], iter[1]);
        }

        return VectorInt2(
            int(ceil(horizontal[1] - horizontal[0])),
            int(ceil(vertical_bounds[1] - vertical_bounds[0]))
            );
    }

    void GenerateGeometry(
        std::vector<uint8_t>& out_vertex_raw_data,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const int in_width_limit_new_line_height,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        auto map_glyph_cell = FindMapGlyphCell(in_font_size);
        SetScale(in_font_size);

        hb_buffer_t* const buffer = MakeBuffer(
            in_string_utf8,
            in_locale_token
            );

        std::vector<PreVertexData> pre_vertex_data;
        VectorFloat2 vertical_bounds(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::min()
            );
        std::vector<VectorFloat2> horizontal_bounds;
        horizontal_bounds.push_back(VectorFloat2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min()));
        ShapeText(
            &pre_vertex_data,
            vertical_bounds,
            horizontal_bounds,
            buffer,
            *map_glyph_cell,
            in_width_limit_enabled,
            in_width_limit,
            in_width_limit_new_line_height
        );
        hb_buffer_destroy(buffer);

        PerformAlign(
            pre_vertex_data,
            vertical_bounds,
            horizontal_bounds,
            in_font_size,
            in_containter_size,
            in_horizontal_line_alignment,
            in_vertical_block_alignment
            );

        BuildVertexData(
            out_vertex_raw_data,
            pre_vertex_data,
            in_containter_size
            );

        return;
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

    void AddVertexData(
        std::vector<PreVertexData>* const out_pre_vertex_data_or_null,
        VectorFloat2& out_vertical_bounds,
        std::vector<VectorFloat2>& out_horizontal_bounds,
        TextCell* in_cell,
        const float in_pos_x,
        const float in_pos_y,
        const int line_index
        )
    {
        const VectorInt2 width_height = in_cell->GetWidthHeight();
        const VectorInt2 bearing = in_cell->GetBearing();
        const VectorFloat4 uv = in_cell->GetUV();
        const VectorFloat4 mask = in_cell->GetMask();

        const float pos_x = in_pos_x + bearing.GetX();
        const float pos_y = in_pos_y - (width_height.GetY() - bearing.GetY());
        const VectorFloat4 pos = VectorFloat4(
            pos_x,
            pos_y,
            pos_x + width_height.GetX(),
            pos_y + width_height.GetY()
            );

        if (nullptr != out_pre_vertex_data_or_null)
        {
            out_pre_vertex_data_or_null->push_back(PreVertexData({
                pos,
                uv,
                mask,
                line_index
                }));
        }

        out_vertical_bounds[0] = std::min(out_vertical_bounds[0], pos[1]);
        out_vertical_bounds[1] = std::max(out_vertical_bounds[1], pos[3]);

        while (line_index < (int)out_horizontal_bounds.size())
        {
            out_horizontal_bounds.push_back(VectorFloat2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min()));
        }

        out_horizontal_bounds[line_index][0] = std::min(out_horizontal_bounds[line_index][0], pos[0]);
        out_horizontal_bounds[line_index][1] = std::max(out_horizontal_bounds[line_index][1], pos[2]);

        return;
    }

    void PerformAlign(
        std::vector<PreVertexData>& in_pre_vertex_data,
        const VectorFloat2& in_vertical_bounds,
        const std::vector<VectorFloat2>& in_horizontal_line_bounds,
        const int in_glyph_size,
        const VectorInt2& in_containter_size,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        float vertical_delta = 0.0f;
        const int line_count = (int)in_horizontal_line_bounds.size();
        std::vector<float> horizontal_line_delta(line_count);
        for (int index = 0; index < line_count; ++index)
        {
            switch (in_horizontal_line_alignment)
            {
            default:
                break;
            case TextEnum::HorizontalLineAlignment::Left:
                break;
            case TextEnum::HorizontalLineAlignment::Middle:
                horizontal_line_delta[index] = (in_containter_size[0] - in_horizontal_line_bounds[index][1]) * 0.5f;
                break;
            case TextEnum::HorizontalLineAlignment::Right:
                horizontal_line_delta[index] = (in_containter_size[0] - in_horizontal_line_bounds[index][1]);
                break;
            }
        }
        switch (in_vertical_block_alignment)
        {
        default:
            break;
        case TextEnum::VerticalBlockAlignment::Bottom:
            break;
        case TextEnum::VerticalBlockAlignment::BottomEM:
            break;
        case TextEnum::VerticalBlockAlignment::Middle:
            vertical_delta = ((in_containter_size[1] + in_containter_size[3]) - (in_vertical_bounds[0] + in_vertical_bounds[1])) * 0.5f;
            break;
        case TextEnum::VerticalBlockAlignment::MiddleEM:
            {
                float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
                vertical_delta = in_containter_size[1] - (temp * 0.5f);
            }
            break;
        case TextEnum::VerticalBlockAlignment::Top:
            vertical_delta = in_containter_size[1] - in_vertical_bounds[1];
            break;
        case TextEnum::VerticalBlockAlignment::TopEM:
            {
                float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
                vertical_delta = in_containter_size[1] - temp;
            }
        break;
        }

        for (auto& item : in_pre_vertex_data)
        {
            const float horizontal_delta = horizontal_line_delta[item._line_index];
            item._pos_low_high[0] += horizontal_delta;
            item._pos_low_high[1] += vertical_delta;
            item._pos_low_high[2] += horizontal_delta;
            item._pos_low_high[3] += vertical_delta;
        }
        return;
    }

    void BuildVertexData(
        std::vector<uint8_t>& out_vertex_raw_data,
        const std::vector<PreVertexData>& in_pre_vertex_data,
        const VectorInt2& in_containter_size
        )
    {
        for (const auto& item : in_pre_vertex_data)
        {
            const VectorFloat4 pos = VectorFloat4(
                ((item._pos_low_high[0] / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
                ((item._pos_low_high[1] / (float)in_containter_size.GetY()) * 2.0f) - 1.0f,
                ((item._pos_low_high[2] / (float)in_containter_size.GetX()) * 2.0f) - 1.0f,
                ((item._pos_low_high[3] / (float)in_containter_size.GetY()) * 2.0f) - 1.0f
                );

            //0.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //0.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //0.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, pos[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
        }

        return;
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
        std::vector<PreVertexData>* const out_pre_vertex_data_or_null,
        VectorFloat2& out_vertical_bounds,
        std::vector<VectorFloat2>& out_horizontal_bounds,
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

        if (nullptr != out_pre_vertex_data_or_null)
        {
            out_pre_vertex_data_or_null->reserve(glyph_count);
        }
        float cursor_x = 0;
        float cursor_y = 0;
        if (true == in_width_limit_enabled)
        {
            // Work out what line each glpyh should be on
            unsigned int current_cluster = std::numeric_limits<unsigned int>::max();// -1;
            std::vector<unsigned int > new_line_clusert_index; // start a new line each time you hit this cluster
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
                            new_line_clusert_index.push_back(glyph_info[ahead].cluster);
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
                    float x_advance = (float)glyph_pos[i].x_advance / 64.0f;
                    cursor_x += x_advance;
                }
            }

            // Now place each glyph
            int line_index = 0;
            cursor_x = 0.0f;
            cursor_y = 0.0f;
            for (unsigned int i = 0; i < glyph_count; i++) 
            {
                // Start new line
                if ((line_index < (int)new_line_clusert_index.size()) &&
                    (new_line_clusert_index[line_index] == glyph_info[i].cluster))
                {
                    line_index += 1;
                    cursor_x = 0.0f;
                    cursor_y += in_width_limit_new_line_height;
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
                AddVertexData(
                    out_pre_vertex_data_or_null,
                    out_vertical_bounds,
                    out_horizontal_bounds,
                    cell, 
                    cursor_x + x_offset, 
                    cursor_y + y_offset,
                    line_index
                    );

                cursor_x += x_advance;
                cursor_y += y_advance;
            }
        }
        else
        {
            for (unsigned int i = 0; i < glyph_count; i++) 
            {
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
                AddVertexData(
                    out_pre_vertex_data_or_null,
                    out_vertical_bounds,
                    out_horizontal_bounds,
                    cell, 
                    cursor_x + x_offset, 
                    cursor_y + y_offset,
                    0
                    );

                cursor_x += x_advance;
                cursor_y += y_advance;
            }
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

VectorInt2 TextFont::CalculateTextBounds(
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const int in_width_limit_new_line_height
    )
{
    return _implementation->CalculateTextBounds(
        in_string_utf8,
        in_locale_token,
        in_font_size,
        in_width_limit_enabled,
        in_width_limit,
        in_width_limit_new_line_height
        );
}

void TextFont::GenerateGeometry(
    std::vector<uint8_t>& out_vertex_raw_data,
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const int in_width_limit_new_line_height,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    _implementation->GenerateGeometry(
        out_vertex_raw_data,
        in_string_utf8,
        in_locale_token,
        in_font_size,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_width_limit_new_line_height,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
    return;
}


void TextFont::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
