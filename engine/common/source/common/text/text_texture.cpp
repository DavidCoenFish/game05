#include "common/common_pch.h"
#include "common/text/text_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader.h"
#include "common/text/text_cell.h"
#include "common/text/text_glyph_row.h"

TextTexture::TextTexture(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const int in_texture_dimention
    )
    : _texture_dirty(false)
    , _texture_dirty_low(0)
    , _texture_dirty_high(0)
    , _texture_dimention(in_texture_dimention)
    , _highest_pos_y{0,0,0,0}
    , _icon_max_pos_y(in_texture_dimention)
{
    D3D12_RESOURCE_DESC desc = {
        D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
        D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
        (UINT64)in_texture_dimention, //UINT64 Width;
        (UINT)in_texture_dimention, //UINT Height;
        1, //UINT16 DepthOrArraySize;
        1, //UINT16 MipLevels;
        // Make it easier to keep mask in sync with data order and use rgba
        //DXGI_FORMAT_B8G8R8A8_UNORM, //DXGI_FORMAT Format;
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

    std::vector<uint8_t> glyph_texture_data;
    glyph_texture_data.resize(in_texture_dimention * in_texture_dimention * 4);

    _texture = in_draw_system->MakeShaderResource(
        in_command_list,
        in_draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
        desc, //const D3D12_RESOURCE_DESC & in_desc,
        shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
        glyph_texture_data
        );
}

TextTexture::~TextTexture()
{
    // Nop
}

std::shared_ptr<TextCell> TextTexture::MakeCell(
    const uint8_t* const in_buffer,
    const uint32_t in_width,
    const uint32_t in_height,
    const uint32_t in_bearing_x,
    const uint32_t in_bearing_y
    )
{
    DSC_ASSERT((int)in_width <= _texture_dimention, "invalid param");
    DSC_ASSERT((int)in_height <= _texture_dimention, "invalid param");

    if ((0 == in_width) || (0 ==in_height))
    {
        return nullptr;
    }

    TextGlyphRow* found_row = nullptr;
    const int desired_height = in_height + (0 != (in_height & 0x03) ? 4 - (in_height & 0x03) : 0);
    for (auto iter = _array_glyph_row.begin(); iter != _array_glyph_row.end(); ++iter)
    {
        TextGlyphRow& row = **iter;
        if ((desired_height == row.GetHeight()) && 
            (row.GetTextureHighestX() + (int)in_width <= _texture_dimention))
        {
            if (row.GetTextureHighestX() + (int)in_width == _texture_dimention)
            {
                _array_glyph_row_full.push_back(*iter);
                _array_glyph_row.erase(iter);
            }

            found_row = &row;
            break;
        }
    }

    // make a new row
    if (nullptr == found_row)
    {
        int mask_index = 0;
        if ((_highest_pos_y[0] <= _highest_pos_y[1]) &&
            (_highest_pos_y[0] <= _highest_pos_y[2]) &&
            (_highest_pos_y[0] <= _highest_pos_y[3]))
        {
            mask_index = 0;
        }
        else if ((_highest_pos_y[1] <= _highest_pos_y[2]) &&
                 (_highest_pos_y[1] <= _highest_pos_y[3]))
        {
            mask_index = 1;
        }
        else if (_highest_pos_y[2] <= _highest_pos_y[3])
        {
            mask_index = 2;
        }
        else
        {
            mask_index = 3;
        }

        if (_icon_max_pos_y < _highest_pos_y[mask_index] + desired_height)
        {
            return nullptr;
        }

        auto temp = std::make_shared<TextGlyphRow>(mask_index, desired_height, _highest_pos_y[mask_index]);
        _highest_pos_y[mask_index] += desired_height;

        if ((int)in_width == _texture_dimention)
        {
            _array_glyph_row_full.push_back(temp);
        }
        else
        {
            _array_glyph_row.push_back(temp);
        }

        found_row = temp.get();
    }

    VectorFloat4 uv = VectorFloat4(
        (float)found_row->GetTextureHighestX() / (float)_texture_dimention,
        (float)(found_row->GetTexturePosY() + in_height) / (float)_texture_dimention,
        (float)(found_row->GetTextureHighestX() + in_width) / (float)_texture_dimention,
        (float)found_row->GetTexturePosY() / (float)_texture_dimention
    );

    VectorFloat4 mask;
    mask[found_row->GetMaskIndex()] = 1.0f;

    // add cell to end of row
    auto cell = std::make_shared<TextCell>(
        VectorInt2(in_width, in_height),
        VectorInt2(in_bearing_x, in_bearing_y),
        uv,
        mask
        );

    _texture_dirty = true;
    _texture_dirty_low = std::min(_texture_dirty_low, found_row->GetTexturePosY());
    _texture_dirty_high = std::max(_texture_dirty_high, found_row->GetTexturePosY() + desired_height);

    auto& dest_data = _texture->GetData();
    for (uint32_t y = 0; y < in_height; ++y)
    {
        for (uint32_t x = 0; x < in_width; ++x)
        {
            const int dest_data_index = ((((found_row->GetTexturePosY() + y) * _texture_dimention) + (found_row->GetTextureHighestX() + x)) * 4) + found_row->GetMaskIndex();
            const int buffer_index = ((y * in_width) + x);
            dest_data[dest_data_index] = in_buffer[buffer_index];
        }
    }

    found_row->IncrementTextureHighestX(in_width);
    return cell;
}

std::shared_ptr<TextCell> TextTexture::MakeIcon(
    const uint8_t* const in_buffer,
    const uint32_t in_width,
    const uint32_t in_height
    )
{
    DSC_ASSERT((int)in_width <= _texture_dimention, "invalid param");
    DSC_ASSERT((int)in_height <= _texture_dimention, "invalid param");

    if ((0 == in_width) || (0 ==in_height))
    {
        return nullptr;
    }

    TextGlyphRow* found_row = nullptr;
    const int desired_height = in_height + (0 != (in_height & 0x03) ? 4 - (in_height & 0x03) : 0);
    for (auto iter = _array_icon_row.begin(); iter != _array_icon_row.end(); ++iter)
    {
        TextGlyphRow& row = **iter;
        if ((desired_height == row.GetHeight()) && 
            (row.GetTextureHighestX() + (int)in_width <= _texture_dimention))
        {
            if (row.GetTextureHighestX() + (int)in_width == _texture_dimention)
            {
                _array_glyph_row_full.push_back(*iter);
                _array_glyph_row.erase(iter);
            }

            found_row = &row;
            break;
        }
    }

    // make a new row
    if (nullptr == found_row)
    {
        // Check icon is not overlapping font
        const int new_max_pos_y = _icon_max_pos_y - in_height;
        if ((new_max_pos_y < _highest_pos_y[0]) ||
            (new_max_pos_y < _highest_pos_y[1]) ||
            (new_max_pos_y < _highest_pos_y[2]) ||
            (new_max_pos_y < _highest_pos_y[3]))
        {
            return nullptr;
        }

        _icon_max_pos_y = new_max_pos_y;

        auto temp = std::make_shared<TextGlyphRow>(-1, desired_height, _icon_max_pos_y);

        if ((int)in_width == _texture_dimention)
        {
            _array_icon_row_full.push_back(temp);
        }
        else
        {
            _array_icon_row.push_back(temp);
        }

        found_row = temp.get();
    }

    VectorFloat4 uv = VectorFloat4(
        (float)found_row->GetTextureHighestX() / (float)_texture_dimention,
        (float)(found_row->GetTexturePosY() + in_height) / (float)_texture_dimention,
        (float)(found_row->GetTextureHighestX() + in_width) / (float)_texture_dimention,
        (float)found_row->GetTexturePosY() / (float)_texture_dimention
    );

    VectorFloat4 mask(1.0f, 1.0f, 1.0f, 1.0f);

    // add cell to end of row
    auto cell = std::make_shared<TextCell>(
        VectorInt2(in_width, in_height),
        VectorInt2(),
        uv,
        mask
        );

    _texture_dirty = true;
    _texture_dirty_low = std::min(_texture_dirty_low, found_row->GetTexturePosY());
    _texture_dirty_high = std::max(_texture_dirty_high, found_row->GetTexturePosY() + desired_height);

    auto& dest_data = _texture->GetData();
    for (uint32_t y = 0; y < in_height; ++y)
    {
        for (uint32_t x = 0; x < in_width; ++x)
        {
            for (uint32_t b = 0; b < 4; ++b)
            {
                const int dest_data_index = ((((found_row->GetTexturePosY() + y) * _texture_dimention) + (found_row->GetTextureHighestX() + x)) * 4) + b;
                const int buffer_index = (((y * in_width) + x) * 4) + b;
                dest_data[dest_data_index] = in_buffer[buffer_index];
            }
        }
    }

    found_row->IncrementTextureHighestX(in_width);
    return cell;
}

void TextTexture::Update(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame
    )
{
    if (false == _texture_dirty)
    {
        return;
    }
    _texture_dirty = false;
    in_draw_system->UploadShaderResource(
        in_draw_system_frame->GetCommandList(),
        _texture.get()
        );
    return;
}


std::shared_ptr<HeapWrapperItem> TextTexture::GetShaderViewHeapWrapperItem() const
{
    return _texture->GetHeapWrapperItem();
}

void TextTexture::RestUsage()
{
    _array_glyph_row.clear();
    _array_glyph_row_full.clear();
    _highest_pos_y[0] = 0;
    _highest_pos_y[1] = 0;
    _highest_pos_y[2] = 0;
    _highest_pos_y[3] = 0;

    _array_icon_row.clear();
    _array_icon_row_full.clear();
    _icon_max_pos_y = _texture_dimention;
}
