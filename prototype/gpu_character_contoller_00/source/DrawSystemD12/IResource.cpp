#include "pch.h"

#include "DrawSystemD12/IResource.h"
#include "DrawSystemD12/DrawSystem.h"

IResource::IResource(DrawSystem* const pDrawSystem)
: m_pDrawSystem(pDrawSystem)
{
	if (nullptr != m_pDrawSystem)
	{
		m_pDrawSystem->AddResource(this);
	}
	return;
}
IResource::~IResource()
{
	if (nullptr != m_pDrawSystem)
	{
		m_pDrawSystem->RemoveResource(this);
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



