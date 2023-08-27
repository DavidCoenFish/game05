#pragma once

class DrawSystem;
class DrawSystemFrame;
class Shader;
class TextBlock;
class TextFont;
class TextLocale;
class TextManagerImplementation;

class TextManager
{
public:
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc();

    TextManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~TextManager();

    // Find or make a new text face
    TextFont* const MakeFont(
        const std::filesystem::path& in_font_rel_path
        );

    void DrawText(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextBlock* const in_text_block
        );

    void UpdateTextBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        TextBlock* const in_text_block
        );

    // Assert if there are any live TextBlocks
    void RestGlyphUsage();

    const TextLocale* const GetLocaleToken(const std::string& in_locale_key) const;

private:
    std::unique_ptr<TextManagerImplementation> _implementation;

};
