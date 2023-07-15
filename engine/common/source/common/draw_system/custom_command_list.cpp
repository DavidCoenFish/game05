#include "common/common_pch.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/d3dx12.h"
#include "common/draw_system/draw_system.h"

CustomCommandList::CustomCommandList(
    DrawSystem&in_draw_system,
    ID3D12GraphicsCommandList* in_command_list
    ) 
    : draw_system(in_draw_system)
    , command_list(in_command_list)
{
    // Nop
}

CustomCommandList::~CustomCommandList()
{
    draw_system.CustomCommandListFinish(in_command_list);
}

ID3D12GraphicsCommandList* CustomCommandList::GetCommandList()
{
    return command_list;
}

