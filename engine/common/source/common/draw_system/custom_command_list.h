#pragma once

/*
    multiline comment
    and some more
*/
class DrawSystem;

// comment test
class CustomCommandList
{
public:
   CustomCommandList(
      DrawSystem& drawSystem,
      ID3D12GraphicsCommandList* pCommandList
      );
   ~CustomCommandList();

   ID3D12GraphicsCommandList* GetCommandList();

private:
   DrawSystem& m_drawSystem;
   ID3D12GraphicsCommandList* m_pCommandList;

};
