// --------------------------------------------------------------------------------------
// File: DirectXHelpers.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#pragma once
#ifdef _GAMING_XBOX_SCARLETT
	#include <d3d12_xs.h>
#elif(defined (_XBOX_ONE) && defined (_TITLE)) || defined (_GAMING_XBOX)
	#include <d3d12_x.h>
#else
	#include <d3d12.h>
#endif

#include <DirectXMath.h>

#include <assert.h>

#include <initializer_list>

#include <utility>

#include <vector>

#include <wrl/client.h>


#ifndef _GAMING_XBOX
	#pragma comment(lib, "dxguid.lib")
#endif

#ifndef IID_GRAPHICS_PPV_ARGS
	#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
#endif

//
// The d3dx12.h header includes the following helper C++ classes and functions
// CD3DX12_RECT
// CD3DX12_BOX
// CD3DX12_DEPTH_STENCIL_DESC
// CD3DX12_BLEND_DESC
// CD3DX12_RASTERIZER_DESC
// CD3DX12_RESOURCE_ALLOCATION_INFO
// CD3DX12_HEAP_PROPERTIES
// CD3DX12_HEAP_DESC
// CD3DX12_CLEAR_VALUE
// CD3DX12_RANGE
// CD3DX12_SHADER_BYTECODE
// CD3DX12_TILED_RESOURCE_COORDINATE
// CD3DX12_TILE_REGION_SIZE
// CD3DX12_SUBRESOURCE_TILING
// CD3DX12_TILE_SHAPE
// CD3DX12_RESOURCE_BARRIER
// CD3DX12_PACKED_MIP_INFO
// CD3DX12_SUBRESOURCE_FOOTPRINT
// CD3DX12_TEXTURE_COPY_LOCATION
// CD3DX12_DESCRIPTOR_RANGE
// CD3DX12_ROOT_DESCRIPTOR_TABLE
// CD3DX12_ROOT_CONSTANTS
// CD3DX12_ROOT_DESCRIPTOR
// CD3DX12_ROOT_PARAMETER
// CD3DX12_STATIC_SAMPLER_DESC
// CD3DX12_ROOT_SIGNATURE_DESC
// CD3DX12_CPU_DESCRIPTOR_HANDLE
// CD3DX12_GPU_DESCRIPTOR_HANDLE
// CD3DX12_RESOURCE_DESC
// D3D12CalcSubresource
// D3D12DecomposeSubresource
// D3D12GetFormatPlaneCount
// MemcpySubresource
// GetRequiredIntermediateSize
// UpdateSubresources
// D3D12IsLayoutOpaque
// CommandListCast
//
namespace DirectX
{
	/// Helpers for aligning values by a power of 2, round down
	template < typename T > inline T AlignDown(
		T in_size,
		size_t in_alignment
		) noexcept
	{
		if (in_alignment > 0)
		{
			assert(((in_alignment - 1) &in_alignment) == 0);
			auto mask = static_cast < T > (in_alignment - 1);
			return in_size&~mask;
		}
		return in_size;
	}

	/// Helpers for aligning values by a power of 2, round up
	template < typename T > inline T AlignUp(
		T in_size,
		size_t in_alignment
		) noexcept
	{
		if (in_alignment > 0)
		{
			assert(((in_alignment - 1) &in_alignment) == 0);
			auto mask = static_cast < T > (in_alignment - 1);
			return (in_size + mask) &~mask;
		}
		return in_size;
	}

	size_t BytesPerPixel(DXGI_FORMAT in_fmt);
}
