#pragma once

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class ShaderResource;
class TextCell;
class TextGlyphRow;

class TextTexture
{
public:
    TextTexture(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_texture_dimention = 2048
        );
    ~TextTexture();

    std::shared_ptr<TextCell> MakeCell(
        const uint8_t* const in_buffer,
        const uint32_t in_width,
        const uint32_t in_height
        );
    void Update(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame
        );

    std::shared_ptr<HeapWrapperItem> GetShaderViewHeapWrapperItem() const;
    void RestGlyphUsage();

private:
    std::shared_ptr<ShaderResource> _glyph_texture;
    //std::vector<uint8_t> _glyph_texture_data;
    // Any "full" TextGlyphRow could be moved to another array, but what is "full", equal to end?
    std::vector<std::shared_ptr<TextGlyphRow>> _array_glyph_row;
    // No, you would still need to visit _array_glyph_row_full to get the highest line in each 
    std::vector<std::shared_ptr<TextGlyphRow>> _array_glyph_row_full;
    int _highest_pos_y[4];

    bool _glyph_texture_dirty;
    int _glyph_texture_dirty_low;
    int _glyph_texture_dirty_high;
    int _texture_dimention;
};
