#pragma once

class DrawSystem;
class HeapWrapperItem;
class ShaderResource;

class TextTexture
{
public:
    TextTexture(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_texture_dimention = 2048
        );
    ~TextTexture();

    std::shared_ptr<HeapWrapperItem> GetShaderViewHeapWrapperItem() const;

private:
    std::shared_ptr<ShaderResource> _glyph_texture;
    std::vector<uint8_t> _glyph_texture_data;
    bool _glyph_texture_dirty;
    int _glyph_texture_dirty_low;
    int _glyph_texture_dirty_high;

};
