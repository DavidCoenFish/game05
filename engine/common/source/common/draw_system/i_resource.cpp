#include "common/common_pch.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/i_resource.h"

#if defined(DRAW_SYSTEM_RESOURCE_ALIVE_COUNT)
int s_draw_system_resource_alive_count = 0;
#endif

IResource::IResource(DrawSystem* const in_draw_system) 
	: _draw_system(in_draw_system)
{
	if (nullptr != _draw_system)
	{
		_draw_system->AddResource(this);
	}

#if defined(DRAW_SYSTEM_RESOURCE_ALIVE_COUNT)
	s_draw_system_resource_alive_count += 1;
#endif

	return;
}

IResource::~IResource()
{
	if (nullptr != _draw_system)
	{
		_draw_system->RemoveResource(this);
	}

#if defined(DRAW_SYSTEM_RESOURCE_ALIVE_COUNT)
	s_draw_system_resource_alive_count -= 1;
#endif

}

void IResource::OnResize(
	ID3D12GraphicsCommandList* const,
	ID3D12Device2* const,
	const VectorInt2&
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

