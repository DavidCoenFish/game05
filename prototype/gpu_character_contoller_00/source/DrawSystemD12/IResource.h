#pragma once

class DrawSystem;
class CustomCommandList;

class IResource
{
public:
   IResource(DrawSystem* const pDrawSystem);
   virtual ~IResource();

   virtual void OnDeviceLost() = 0;
   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice
      ) = 0;

   virtual void OnResize(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice,
      const int screenWidth,
      const int screenHeight
      );

protected:
   DrawSystem* m_pDrawSystem;

};
