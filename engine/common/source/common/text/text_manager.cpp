#include "common/common_pch.h"
#include "common/text/text_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/file_system/file_system.h"
#include "common/text/text_block.h"
#include "common/text/text_face.h"
#include "common/text/text_geometry.h"
#include "common/text/text_row.h"
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
}

class TextManagerImplementation
{
public:
    TextManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
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
        //DrawSystemFrame* const in_draw_system_frame,
        ID3D12GraphicsCommandList* const in_command_list,
        TextBlock* const in_text_block
        )
    {
        in_text_block->Update(
            in_draw_system,
            in_command_list,
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

    std::shared_ptr<TextTexture> _texture;

    std::vector<std::shared_ptr<TextFace>> _array_text_face;

    // The array of glyph data in the shader glyph texture
    std::vector<std::shared_ptr<TextRow>> _array_text_row;

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
    //DrawSystemFrame* const in_draw_system_frame,
    ID3D12GraphicsCommandList* const in_command_list,
    TextBlock* const in_text_block
    )
{
    _implementation->UpdateTextBlock(
        in_draw_system,
        //in_draw_system_frame,
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
