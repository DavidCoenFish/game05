#pragma once
/*
    multiline comment
    and some more
*/
class DrawSystem;

// Comment test
class CustomCommandList
{
public:
    CustomCommandList(
        DrawSystem&in_draw_system,
        ID3D12GraphicsCommandList* in_command_list
        );
    ~CustomCommandList();
    ID3D12GraphicsCommandList* GetCommandList();

private:
    ID3D12GraphicsCommandList* _command_list;
    DrawSystem&_draw_system;
};
