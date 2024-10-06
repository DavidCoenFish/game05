#pragma once
class DrawSystem;

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
	DrawSystem& _draw_system;
	ID3D12GraphicsCommandList* _command_list;
};
