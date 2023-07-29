#pragma once

class DrawSystem;
class DrawSystemFrame;
class Shader;
class TextBlock;
class TextFace;
class TextManagerImplementation;

class TextManager
{
public:
    TextManager(
        DrawSystem* const in_draw_system,
        const std::filesystem::path& in_shader_root_path
        );

    std::shared_ptr<TextFace> MakeTextFace(
        const std::filesystem::path& in_font_file_path
        );

    void UpdateTextBlock(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextBlock* const in_text_block
        );

    // Assert if there are any TextGeometry
    void RestGlyphUsage();

private:
    std::unique_ptr<TextManagerImplementation> _implementation;

};
