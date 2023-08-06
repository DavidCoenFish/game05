#include "common/common_pch.h"
#include "common/text/text_face.h"

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
    };
}

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

    ~TextFaceImplementation()
    {
        hb_font_destroy(_harf_buzz_font);
        FT_Done_Face(_face);
    }

    std::shared_ptr<TextBlock> MakeBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
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

        hb_buffer_t* const buffer = MakeBuffer(
            in_string_utf8,
            in_locale_token
            );

        std::vector<PreVertexData> pre_vertex_data;
        VectorFloat4 bounds(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min()
            );
        ShapeText(
            &pre_vertex_data,
            bounds,
            buffer,
            *map_glyph_cell
            );
        hb_buffer_destroy(buffer);

        PerformAlign(
            pre_vertex_data,
            bounds,
            in_glyph_size,
            in_containter_size,
            in_horizontal_line_alignment,
            in_vertical_block_alignment
            );

        std::vector<uint8_t> vertex_raw_data;
        BuildVertexData(
            vertex_raw_data,
            pre_vertex_data,
            in_containter_size
            );

        auto result = std::make_shared<TextBlock>(
            in_draw_system,
            in_command_list,
            in_string_utf8,
            in_locale_token,
            in_containter_size,
            in_width_limit_enabled,
            in_width_limit,
            in_horizontal_line_alignment,
            in_vertical_block_alignment,
            vertex_raw_data
            );

        return result;
    }

    VectorFloat2 CalculateTextBounds(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size
        )
    {
        auto map_glyph_cell = FindMapGlyphCell(in_font_size);
        SetScale(in_font_size);

        hb_buffer_t* const buffer = MakeBuffer(
            in_string_utf8,
            in_locale_token
            );

        std::vector<PreVertexData> pre_vertex_data;
        VectorFloat4 bounds(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min()
        );
        ShapeText(
            &pre_vertex_data,
            bounds,
            buffer,
            *map_glyph_cell
        );
        hb_buffer_destroy(buffer);

        return VectorFloat2(
            bounds[2] - bounds[0],
            bounds[3] - bounds[1]
            );
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
        VectorFloat4& out_pos_bounds,
        TextCell* in_cell,
        const float in_pos_x,
        const float in_pos_y
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
                mask
                }));
        }

        out_pos_bounds[0] = std::min(out_pos_bounds[0], pos[0]);
        out_pos_bounds[1] = std::min(out_pos_bounds[1], pos[1]);
        out_pos_bounds[2] = std::max(out_pos_bounds[2], pos[2]);
        out_pos_bounds[3] = std::max(out_pos_bounds[3], pos[3]);

        return;
    }

    void PerformAlign(
        std::vector<PreVertexData>& in_pre_vertex_data,
        const VectorFloat4& in_bounds,
        const int in_glyph_size,
        const VectorInt2& in_containter_size,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        )
    {
        VectorFloat2 delta;
        switch (in_horizontal_line_alignment)
        {
        default:
            break;
        case TextEnum::HorizontalLineAlignment::Left:
            break;
        case TextEnum::HorizontalLineAlignment::Middle:
            delta[0] = (in_containter_size[0] - in_bounds[2]) * 0.5f;
            break;
        case TextEnum::HorizontalLineAlignment::Right:
            delta[0] = (in_containter_size[0] - in_bounds[2]);
            break;
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
            delta[1] = ((in_containter_size[1] + in_containter_size[3]) - (in_bounds[1] + in_bounds[3])) * 0.5f;
            break;
        case TextEnum::VerticalBlockAlignment::MiddleEM:
            {
                float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
                delta[1] = in_containter_size[1] - (temp * 0.5f);
            }
            break;
        case TextEnum::VerticalBlockAlignment::Top:
            delta[1] = in_containter_size[1] - in_bounds[3];
            break;
        case TextEnum::VerticalBlockAlignment::TopEM:
            {
                float temp = ((float)(in_glyph_size / 2)) / ((float)in_containter_size[1]);
                delta[1] = in_containter_size[1] - temp;
            }
        break;
        }

        for (auto& item : in_pre_vertex_data)
        {
            item._pos_low_high[0] += delta[0];
            item._pos_low_high[1] += delta[1];
            item._pos_low_high[2] += delta[0];
            item._pos_low_high[3] += delta[1];
        }
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

#if 1
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

#else
            // Warning, inverted Y
            //0.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[1]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[1]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //0.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[3]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 0.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[1]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[1]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //1.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[3]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[2]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);

            //0.0f, 1.0f,
            VectorHelper::AppendValue(out_vertex_raw_data, pos[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, -(pos[3]));
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[0]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._uv_low_high[3]);
            VectorHelper::AppendValue(out_vertex_raw_data, item._mask);
#endif
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

    //std::vector<PreVertexData> pre_vertex_data;
    //VectorFloat4 bounds(1.0f, 1.0f, -1.0f, -1.0f);

    void ShapeText(
        std::vector<PreVertexData>* const out_pre_vertex_data_or_null,
        VectorFloat4& out_bounds,
        hb_buffer_t* in_buffer,
        TMapGlyphCell& in_out_map_glyph_cell
        //std::vector<uint8_t>& out_vertex_raw_data,
        //const VectorInt2& in_containter_size,
        //const int in_glyph_size,
        //const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        //const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
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
        for (unsigned int i = 0; i < glyph_count; i++) 
        {
            hb_codepoint_t codepoint = glyph_info[i].codepoint;
            //LOG_MESSAGE_DEBUG("ShapeText2[%d]", glyph_info[i].codepoint);

            float x_offset = (float)glyph_pos[i].x_offset / 64.0f;
            float y_offset = (float)glyph_pos[i].y_offset / 64.0f;
            float x_advance = (float)glyph_pos[i].x_advance / 64.0f;
            float y_advance = (float)glyph_pos[i].y_advance / 64.0f;
            // draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
            FT_Error error = FT_Load_Glyph(_face, codepoint, FT_LOAD_DEFAULT);
            error;
            FT_GlyphSlot slot = _face->glyph;
            FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

            auto cell = FindOrMakeCell(codepoint, slot, in_out_map_glyph_cell);
            AddVertexData(
                out_pre_vertex_data_or_null,
                out_bounds,
                cell, 
                cursor_x + x_offset, 
                cursor_y + y_offset
                );

            cursor_x += x_advance;
            cursor_y += y_advance;
        }

        return;
    }

private:
    FT_Face _face;
    hb_font_t* _harf_buzz_font;
    //std::vector<hb_feature_t> _features;

    TextTexture* _text_texture;

    // For each font size, have a glyph cell referencing the uv on _text_texture
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
    const TextLocale* const in_locale_token,
    const int in_font_size,
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
        in_locale_token,
        in_font_size,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

VectorFloat2 TextFace::CalculateTextBounds(
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const int in_font_size
    )
{
    return _implementation->CalculateTextBounds(
        in_string_utf8,
        in_locale_token,
        in_font_size
        );
}

void TextFace::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
