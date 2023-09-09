#pragma once

class DrawSystem;
class DrawSystemFrame;
class IconFont;
class Shader;
class TextBlock;
class TextFont;
class TextLocale;
class TextManagerImplementation;
class TextRun;
enum class LocaleISO_639_1;

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

    void DrawTextRun(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextRun* const in_text_run
        );

    // An Icon is a small 4 channel image
    IconFont* const GetIconFont();

    //void AddIcon(
    //    const int in_icon_id,
    //    const int in_width,
    //    const int in_height,
    //    const uint8_t* const in_buffer
    //    );

    // Assert if there are any live TextBlocks, clear the font/icon texture
    void RestUsage();

    const TextLocale* const GetLocaleToken(const LocaleISO_639_1 in_locale) const;

private:
    std::unique_ptr<TextManagerImplementation> _implementation;

};
