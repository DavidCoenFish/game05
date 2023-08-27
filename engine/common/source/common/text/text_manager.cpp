#include "common/common_pch.h"
#include "common/text/text_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/file_system/file_system.h"
#include "common/text/text_block.h"
#include "common/text/text_font.h"
#include "common/text/text_glyph_row.h"
#include "common/text/text_locale.h"
#include "common/text/text_texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
    static const std::vector<D3D12_INPUT_ELEMENT_DESC> s_input_element_desc_array({
        D3D12_INPUT_ELEMENT_DESC
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        D3D12_INPUT_ELEMENT_DESC
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        }
    });

    static std::shared_ptr<TextLocale> s_locale_en = std::make_shared<TextLocale>(
        HB_DIRECTION_LTR,
        HB_SCRIPT_LATIN,
        hb_language_from_string("en", -1)
        );
    static std::map<std::string, std::shared_ptr<TextLocale>> s_map_text_locale({
        {"ar", std::make_shared<TextLocale>(
            HB_DIRECTION_RTL,
            HB_SCRIPT_ARABIC,
            hb_language_from_string("ar", -1)
        )},
        {"ch", std::make_shared<TextLocale>(
            HB_DIRECTION_LTR, //HB_DIRECTION_TTB,
            HB_SCRIPT_HAN,
            hb_language_from_string("ch", -1)
        )},
        {"en", s_locale_en},
        {"fr", std::make_shared<TextLocale>(
            HB_DIRECTION_LTR,
            HB_SCRIPT_LATIN,
            hb_language_from_string("fr", -1)
        )},
        {"hi", std::make_shared<TextLocale>(
            HB_DIRECTION_LTR, //HB_DIRECTION_TTB,
            HB_SCRIPT_DEVANAGARI,
            hb_language_from_string("hi", -1)
        )},
        {"ru", std::make_shared<TextLocale>(
            HB_DIRECTION_LTR,
            HB_SCRIPT_CYRILLIC,
            hb_language_from_string("ru", -1)
        )},
    });
}

class TextManagerImplementation
{
public:
    TextManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
        : _root_path(in_root_path)
    {
        FT_Error error;
        error = FT_Init_FreeType(&_library);

        _texture = std::make_shared<TextTexture>(
            in_draw_system,
            in_command_list
            );

        {
            auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "text_vertex.cso");
            auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "text_pixel.cso");
            std::vector<DXGI_FORMAT> render_target_format;
            render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
            const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();
            ShaderPipelineStateData shader_pipeline_state_data(
                s_input_element_desc_array,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                DXGI_FORMAT_UNKNOWN,
                render_target_format,
                blend_desc,
                CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                CD3DX12_DEPTH_STENCIL_DESC()
                );

            std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
            array_shader_resource_info.push_back(ShaderResourceInfo::FactoryDataSampler(
                _texture->GetShaderViewHeapWrapperItem(),
                D3D12_SHADER_VISIBILITY_PIXEL
                ));

            std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info;

            _shader = in_draw_system->MakeShader(
                in_command_list,
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data,
                array_shader_resource_info,
                array_shader_constants_info
                );
        }

    }

    ~TextManagerImplementation()
    {
        _map_path_font.clear();
        FT_Done_FreeType(_library);
    }

    TextFont* const MakeFont(
        const std::filesystem::path& in_font_rel_path
        )
    {
        auto found = _map_path_font.find(in_font_rel_path);
        if (found != _map_path_font.end())
        {
            return found->second.get();
        }

        const std::filesystem::path font_path = _root_path / in_font_rel_path;

        auto text_face = std::make_shared<TextFont>(_library, font_path, _texture.get());
        _map_path_font[in_font_rel_path] = text_face;

        return text_face.get();
    }

    void UpdateTextBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        TextBlock* const in_text_block
        )
    {
        bool geometry_dirty = false;
        if (nullptr != in_text_block)
        {
            in_text_block->GetGeometry(
                geometry_dirty,
                in_draw_system,
                in_command_list
                );
        }
    }

    void DrawText(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextBlock* const in_text_block
        )
    {
        bool geometry_dirty = false;
        auto geometry = in_text_block->GetGeometry(
            geometry_dirty,
            in_draw_system,
            in_draw_system_frame->GetCommandList()
            );
        _texture->Update(in_draw_system, in_draw_system_frame);
        in_draw_system_frame->SetShader(_shader.get());
        in_draw_system_frame->Draw(geometry);
    }

    // Assert if there are any TextGeometry?
    void RestGlyphUsage()
    {
        for (auto iter : _map_path_font)
        {
            iter.second->RestGlyphUsage();
        }
        _texture->RestGlyphUsage();
    }

    const TextLocale* const GetLocaleToken(const std::string& in_locale_key) const
    {
        const auto found = s_map_text_locale.find(in_locale_key);
        if (found != s_map_text_locale.end())
        {
            return found->second.get();
        }
        return s_locale_en.get();
    }

private:
    std::filesystem::path _root_path;
    FT_Library _library;

    std::map<std::filesystem::path, std::shared_ptr<TextFont>> _map_path_font;

    std::shared_ptr<Shader> _shader;
    std::shared_ptr<TextTexture> _texture;

};

const std::vector<D3D12_INPUT_ELEMENT_DESC>& TextManager::GetInputElementDesc()
{
    return s_input_element_desc_array;
}

TextManager::TextManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_root_path
    )
{
    _implementation = std::make_unique<TextManagerImplementation>(
        in_draw_system,
        in_command_list,
        in_root_path
        );
}

TextManager::~TextManager()
{
    // Nop
}

TextFont* const TextManager::MakeFont(
    const std::filesystem::path& in_font_rel_path
    )
{
    return _implementation->MakeFont(
        in_font_rel_path
        );
}

void TextManager::DrawText(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    TextBlock* const in_text_block
    )
{
    _implementation->DrawText(
        in_draw_system,
        in_draw_system_frame,
        in_text_block
        );
}

void TextManager::UpdateTextBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    TextBlock* const in_text_block
    )
{
    _implementation->UpdateTextBlock(
        in_draw_system,
        in_command_list,
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

const TextLocale* const TextManager::GetLocaleToken(const std::string& in_locale_key) const
{
    return _implementation->GetLocaleToken(in_locale_key);
}

