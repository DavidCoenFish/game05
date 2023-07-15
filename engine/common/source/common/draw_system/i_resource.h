#pragma once
class DrawSystem;
class CustomCommandList;

class IResource
{
public:
    IResource(DrawSystem* const in_draw_system);
    virtual ~IResource();
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) = 0;
    virtual void OnResize(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device,
        const int in_screen_width,
        const int in_screen_height
        );

protected:
    DrawSystem* _draw_system;
};
