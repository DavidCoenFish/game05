#include "common/common_pch.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/i_resource.h"

IResource::IResource(DrawSystem* const in_draw_system) 
    : _draw_system(in_draw_system)
{
    if (nullptr != _draw_system)
    {
        _draw_system->AddResource(this);
    }
    return;
}

IResource::~IResource()
{
    if (nullptr != _draw_system)
    {
        _draw_system->RemoveResource(this);
    }
}

void IResource::OnResize(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const,
    const int,
    const int
    )
{
    return;
}

void IResource::OnResourceBarrier(
    ID3D12GraphicsCommandList* const,
    D3D12_RESOURCE_STATES
    )
{
    return;
}

