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
    // Constexpr D3D12_CPU_DESCRIPTOR_HANDLE D3D12_CPU_DESCRIPTOR_HANDLE_ZERO = {};
    //
    // // Creates a shader resource view from an arbitrary resource
    // Void __cdecl CreateShaderResourceView(
    // _In_ ID3D12Device* device,
    // _In_ ID3D12Resource* tex,
    // D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor,
    // Bool isCubeMap = false);
    //
    // // Shorthand for creating a root signature
    // Inline HRESULT CreateRootSignature(
    // _In_ ID3D12Device* device,
    // _In_ const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
    // _Out_ ID3D12RootSignature** rootSignature) noexcept
    // {
    // Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
    // Microsoft::WRL::ComPtr<ID3DBlob> pError;

        // HRESULT hr = D3D12SerializeRootSignature(rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
    // If (SUCCEEDED(hr))
    // {
    // Hr = device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
    // IID_GRAPHICS_PPV_ARGS(rootSignature)
    // );
    // }
    // Return hr;
    // }
    //
    // // Helper for obtaining texture size
    // Inline XMUINT2 GetTextureSize(_In_ ID3D12Resource* tex) noexcept
    // {
    // Const auto desc = tex->GetDesc();
    // Return XMUINT2(static_cast<uint32_t>(desc.Width), static_cast<uint32_t>(desc.Height));
    // }
    //
    // #if defined(_PIX_H_) || defined(_PIX3_H_)
    // // Scoped PIX event.
    // Class ScopedPixEvent
    // {
    // Public:
    // ScopedPixEvent(_In_ ID3D12GraphicsCommandList* pCommandList, UINT64 /*metadata*/, PCWSTR pFormat) noexcept
    // : mCommandList(pCommandList)
    // {
    // PIXBeginEvent(pCommandList, 0, pFormat);
    // }
    // ~ScopedPixEvent()
    // {
    // PIXEndEvent(mCommandList);
    // }
    //
    // Private:
    // ID3D12GraphicsCommandList* mCommandList;
    // };
    // #endif
    //
    // // Helper sets a D3D resource name string (used by PIX and debug layer leak reporting).
    // Template<UINT TNameLength>
    // Inline void SetDebugObjectName(_In_ ID3D12DeviceChild* resource, _In_z_ const char(&name)[TNameLength]) noexcept
    // {
    // #if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
    // Wchar_t wname[MAX_PATH];
    // Int result = MultiByteToWideChar(CP_UTF8, 0, name, TNameLength, wname, MAX_PATH);
    // If (result > 0)
    // {
    // Resource->SetName(wname);
    // }
    // #else
    // UNREFERENCED_PARAMETER(resource);
    // UNREFERENCED_PARAMETER(name);
    // #endif
    // }
    //
    // Template<UINT TNameLength>

        // Inline void SetDebugObjectName(_In_ ID3D12DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength]) noexcept
    // {
    // #if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
    // Resource->SetName(name);
    // #else
    // UNREFERENCED_PARAMETER(resource);
    // UNREFERENCED_PARAMETER(name);
    // #endif
    // }
    //
    // // Helper for resource barrier.
    // Inline void TransitionResource(
    // _In_ ID3D12GraphicsCommandList* commandList,
    // _In_ ID3D12Resource* resource,
    // D3D12_RESOURCE_STATES stateBefore,
    // D3D12_RESOURCE_STATES stateAfter) noexcept
    // {
    // Assert(commandList != nullptr);
    // Assert(resource != nullptr);
    //
    // If (stateBefore == stateAfter)
    // Return;
    //
    // D3D12_RESOURCE_BARRIER desc = {};
    // Desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // Desc.Transition.pResource = resource;
    // Desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    // Desc.Transition.StateBefore = stateBefore;
    // Desc.Transition.StateAfter = stateAfter;
    //
    // CommandList->ResourceBarrier(1, &desc);
    // }
    //
    // // Helper which applies one or more resources barriers and then reverses them on destruction.
    // Class ScopedBarrier
    // {
    // Public:
    // ScopedBarrier(
    // _In_ ID3D12GraphicsCommandList* commandList,
    // Std::initializer_list<D3D12_RESOURCE_BARRIER> barriers) noexcept(false)
    // : mCommandList(commandList),
    // MBarriers(barriers)
    // {
    // Assert(mBarriers.size() <= UINT32_MAX);
    //
    // // Set barriers
    // MCommandList->ResourceBarrier(static_cast<UINT>(mBarriers.size()), mBarriers.data());
    // }
    //
    // ScopedBarrier(ScopedBarrier&&) = default;
    // ScopedBarrier& operator= (ScopedBarrier&&) = default;
    //
    // ScopedBarrier(ScopedBarrier const&) = delete;
    // ScopedBarrier& operator= (ScopedBarrier const&) = delete;
    //
    // ~ScopedBarrier()
    // {
    // // reverse barrier inputs and outputs
    // For (auto& b : mBarriers)
    // {
    // Std::swap(b.Transition.StateAfter, b.Transition.StateBefore);
    // }
    //
    // // Set barriers
    // MCommandList->ResourceBarrier(static_cast<UINT>(mBarriers.size()), mBarriers.data());
    // }
    //
    // Private:
    // ID3D12GraphicsCommandList* mCommandList;
    // Std::vector<D3D12_RESOURCE_BARRIER> mBarriers;
    // };
    //
    // // Helper to check for power-of-2
    // Template<typename T>
    // Constexpr bool IsPowerOf2(T x) noexcept { return ((x != 0) && !(x & (x - 1))); }
    //
    // Helpers for aligning values by a power of 2
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
