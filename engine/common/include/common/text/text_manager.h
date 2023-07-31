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
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc();

    TextManager(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        );
    ~TextManager();

    std::shared_ptr<TextFace> MakeTextFace(
        const std::filesystem::path& in_font_file_path
        );

    void DrawText(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        TextBlock* const in_text_block
        );

    void UpdateTextBlock(
        DrawSystem* const in_draw_system,
        //DrawSystemFrame* const in_draw_system_frame,
        ID3D12GraphicsCommandList* const in_command_list,
        TextBlock* const in_text_block
        );

    // Assert if there are any TextGeometry
    void RestGlyphUsage();

private:
    std::unique_ptr<TextManagerImplementation> _implementation;

};
