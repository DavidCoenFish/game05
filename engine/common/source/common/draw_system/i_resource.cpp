#include "common/common_pch.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/i_resource.h"

IResource::IResource(DrawSystem* const in_draw_system) 
    : draw_system(in_draw_system)
{
    if (nullptr != draw_system)
    {
        draw_system->AddResource(this);
    }
    return;
}

IResource::~IResource()
{
    if (nullptr != draw_system)
    {
        draw_system->RemoveResource(this);
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

