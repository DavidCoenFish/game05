#include "common/common_pch.h"
#include "common/text/text_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/shader/shader_resource.h"

TextTexture::TextTexture(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const int in_texture_dimention
    )
    : _glyph_texture_dirty(false)
    , _glyph_texture_dirty_low(0)
    , _glyph_texture_dirty_high(0)
{
    D3D12_RESOURCE_DESC desc = {
        D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
        D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
        (UINT64)in_texture_dimention, //UINT64 Width;
        (UINT)in_texture_dimention, //UINT Height;
        1, //UINT16 DepthOrArraySize;
        1, //UINT16 MipLevels;
        DXGI_FORMAT_R8G8B8A8_UNORM, //DXGI_FORMAT Format;
        DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
        D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
        D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
    };
    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
    shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    shader_resource_view_desc.Format = desc.Format;
    shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    _glyph_texture = in_draw_system->MakeShaderResource(
        in_command_list,
        in_draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
        desc, //const D3D12_RESOURCE_DESC & in_desc,
        shader_resource_view_desc //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
        );
}

TextTexture::~TextTexture()
{
    // Nop
}

std::shared_ptr<HeapWrapperItem> TextTexture::GetShaderViewHeapWrapperItem() const
{
    return _glyph_texture->GetHeapWrapperItem();
}
