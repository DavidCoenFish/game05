#include "common/common_pch.h"

#include "common/draw_system/custom_command_list.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"

CustomCommandList::CustomCommandList(
	DrawSystem&in_draw_system,
	ID3D12GraphicsCommandList* in_command_list
	) 
	: _draw_system(in_draw_system)
	, _command_list(in_command_list)
{
	// Nop
}

CustomCommandList::~CustomCommandList()
{
	_draw_system.CustomCommandListFinish(_command_list);
}

ID3D12GraphicsCommandList* CustomCommandList::GetCommandList()
{
	return _command_list;
}

