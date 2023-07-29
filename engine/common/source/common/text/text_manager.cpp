#include "common/common_pch.h"
#include "common/text/text_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/text/text_block.h"
#include "common/text/text_face.h"
#include "common/text/text_geometry.h"
#include "common/text/text_row.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class TextManagerImplementation
{
public:
    TextManagerImplementation(
        DrawSystem* const in_draw_system,
        const std::filesystem::path& in_shader_root_path,
        const int _texture_size = 2048
        )
    {
        FT_Error error;
        error = FT_Init_FreeType(&_library);

    }

    ~TextManagerImplementation()
    {
        FT_Done_FreeType(_library);
    }

    std::shared_ptr<TextFace> MakeTextFace(
        const std::filesystem::path& in_font_file_path
        )
    {
        auto found = _map_path_face.find(in_font_file_path);
        if (found != _map_path_face.end())
        {
            return found->second;
        }

        auto text_face = std::make_shared<TextFace>(_library, in_font_file_path);
        _map_path_face[in_font_file_path] = text_face;

        return text_face;
    }

    void UpdateTextBlock(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextBlock* const in_text_block
        )
    {
        in_text_block->Update(
            in_draw_system,
            in_draw_system_frame,
            _shader.get()
            );
    }

    // Assert if there are any TextGeometry
    void RestGlyphUsage()
    {
        for (auto iter : _array_text_face)
        {
            iter->RestGlyphUsage();
        }
        //_array_text_row.reset();
    }

private:
    FT_Library _library;
    std::map<std::filesystem::path, std::shared_ptr<TextFace>> _map_path_face;

    std::shared_ptr<Shader> _shader;

    std::shared_ptr<ShaderResource> _glyph_texture;
    std::vector<uint8_t> _glyph_texture_data;
    bool _glyph_texture_dirty;
    VectorInt2 _glyph_texture_dirty_range;

    std::vector<std::shared_ptr<TextFace>> _array_text_face;

    // The array of glyph data in the shader glyph texture
    std::vector<std::shared_ptr<TextRow>> _array_text_row;

};

TextManager::TextManager(
    DrawSystem* const in_draw_system,
    const std::filesystem::path& in_shader_root_path
    )
{
    _implementation = std::make_unique<TextManagerImplementation>(
        in_draw_system,
        in_shader_root_path
        );
}

std::shared_ptr<TextFace> TextManager::MakeTextFace(
    const std::filesystem::path& in_font_file_path
    // flag
    )
{
    return _implementation->MakeTextFace(
        in_font_file_path
        );
}

void TextManager::UpdateTextBlock(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    TextBlock* const in_text_block
    )
{
    _implementation->UpdateTextBlock(
        in_draw_system,
        in_draw_system_frame,
        in_text_block
        );
    return;
}

// Assert if there are any TextGeometry
void TextManager::RestGlyphUsage()
{
    _implementation->RestGlyphUsage();
    return;
}
