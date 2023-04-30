#include "pch.h"

#include "DrawSystemD12/CustomCommandList.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/d3dx12.h"

CustomCommandList::CustomCommandList(
   DrawSystem& drawSystem,
   ID3D12GraphicsCommandList* pCommandList
   )
   : m_drawSystem(drawSystem)
   , m_pCommandList(pCommandList)
{
   //nop
}

CustomCommandList::~CustomCommandList()
{
   m_drawSystem.CustomCommandListFinish(m_pCommandList);
}

ID3D12GraphicsCommandList* CustomCommandList::GetCommandList()
{
   return m_pCommandList;
}
