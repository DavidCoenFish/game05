// --------------------------------------------------------------------------------------
// File: GraphicsMemory.h
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

#include <memory>


namespace DirectX
{
    class LinearAllocatorPage;

    /// Works a little like a smart pointer. The memory will only be fenced by the GPU once the pointer
    /// Has been invalidated or the user explicitly marks it for fencing.
    class GraphicsResource
    {
    public:
        GraphicsResource() noexcept;
        GraphicsResource(
            _In_ LinearAllocatorPage* in_page,
            _In_ D3D12_GPU_VIRTUAL_ADDRESS in_gpu_address,
            _In_ ID3D12Resource* in_resource,
            _In_ void* in_memory,
            _In_ size_t in_offset,
            _In_ size_t in_size
            ) noexcept;
        GraphicsResource(GraphicsResource && in_other) noexcept;
        GraphicsResource &&  operator=(GraphicsResource &&) noexcept;
        GraphicsResource(const GraphicsResource&) = delete;
        GraphicsResource& operator=(const GraphicsResource&) = delete;
        ~GraphicsResource();
        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept
        {
            return _gpu_address;
        }

        ID3D12Resource* Resource() const noexcept
        {
            return _resource;
        }

        void* Memory() const noexcept
        {
            return _memory;
        }

        size_t ResourceOffset() const noexcept
        {
            return _buffer_offset;
        }

        size_t Size() const noexcept
        {
            return _size;
        }

        explicit operator bool () const noexcept
        {
            return _resource != nullptr;
        }

        /// Clear the pointer. Using operator -> will produce bad results.
        void __cdecl Reset() noexcept;
        void __cdecl Reset(GraphicsResource &&) noexcept;

    private:
        LinearAllocatorPage* _page;
        D3D12_GPU_VIRTUAL_ADDRESS _gpu_address;
        ID3D12Resource* _resource;
        void* _memory;
        size_t _buffer_offset;
        size_t _size;
    };
    class SharedGraphicsResource
    {
    public:
        SharedGraphicsResource() noexcept;
        SharedGraphicsResource(SharedGraphicsResource &&) noexcept;
        SharedGraphicsResource && operator=(SharedGraphicsResource &&) noexcept;
        SharedGraphicsResource(GraphicsResource &&);
        SharedGraphicsResource && operator=(GraphicsResource &&);
        SharedGraphicsResource(const SharedGraphicsResource&) noexcept;
        SharedGraphicsResource& operator=(const SharedGraphicsResource&) noexcept;
        SharedGraphicsResource(const GraphicsResource&) = delete;
        SharedGraphicsResource& operator=(const GraphicsResource&) = delete;
        ~SharedGraphicsResource();
        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept
        {
            return _shared_resource->GpuAddress();
        }

        ID3D12Resource* Resource() const noexcept
        {
            return _shared_resource->Resource();
        }

        void* Memory() const noexcept
        {
            return _shared_resource->Memory();
        }

        size_t ResourceOffset() const noexcept
        {
            return _shared_resource->ResourceOffset();
        }

        size_t Size() const noexcept
        {
            return _shared_resource->Size();
        }

        explicit operator bool () const noexcept
        {
            return _shared_resource != nullptr;
        }

        bool operator == (const SharedGraphicsResource&in_other) const noexcept
        {
            return _shared_resource.get() == in_other._shared_resource.get();
        }

        bool operator != (const SharedGraphicsResource&in_other) const noexcept
        {
            return _shared_resource.get() != in_other._shared_resource.get();
        }

        /// Clear the pointer. Using operator -> will produce bad results.
        void __cdecl Reset() noexcept;
        void __cdecl Reset(GraphicsResource &&);
        void __cdecl Reset(SharedGraphicsResource &&) noexcept;
        void __cdecl Reset(const SharedGraphicsResource&in_resource) noexcept;

    private:
        std::shared_ptr < GraphicsResource > _shared_resource;
    };
    // ----------------------------------------------------------------------------------
    struct GraphicsMemoryStatistics
    {
    public:
        size_t _committed_memory;
        // Bytes of memory currently committed/in-flight
        size_t _total_memory;
        // Total bytes of memory used by the allocators
        size_t _total_pages;
        // Total page count
        size_t _peak_commited_memory;
        // Peak commited memory value since last reset
        size_t _peak_total_memory;
        // Peak total bytes
        size_t _peak_total_pages;
    };
    // ----------------------------------------------------------------------------------
    class GraphicsMemory
    {
    public:
        explicit GraphicsMemory(_In_ ID3D12Device* in_device);
        GraphicsMemory(GraphicsMemory && in_move_from) noexcept;
        GraphicsMemory& operator=(GraphicsMemory && in_move_from) noexcept;
        GraphicsMemory(GraphicsMemory const&) = delete;
        GraphicsMemory& operator=(GraphicsMemory const&) = delete;
        /// Singleton
        /// Should only use nullptr for single GPU scenarios; mGPU requires a specific device
        static GraphicsMemory& __cdecl Get(_In_opt_ ID3D12Device* in_device = nullptr);

        virtual ~GraphicsMemory();
        /// Make sure to keep the GraphicsResource handle alive as long as you need to access
        /// The memory on the CPU. For example, do not simply cache GpuAddress() and discard
        /// The GraphicsResource object, or your memory may be overwritten later.
        GraphicsResource __cdecl Allocate(
            size_t in_size,
            size_t in_alignment = 16
            );
        /// Special overload of Allocate that aligns to D3D12 constant buffer alignment requirements
        template < typename T > GraphicsResource AllocateConstant()
        {
            const size_t in_alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
            const size_t aligned_size = (sizeof (T) + in_alignment - 1) &~ (in_alignment - 1);
            return Allocate(
                aligned_size,
                in_alignment
                );
        }

        template < typename T > GraphicsResource AllocateConstant(const T&in_set_data)
        {
            GraphicsResource alloc = AllocateConstant < T > ();
            memcpy(
                alloc.Memory(),
                &in_set_data,
                sizeof (T)
                );
            return alloc;
        }

        /// Submits all the pending one-shot memory to the GPU.
        /// The memory will be recycled once the GPU is done with it.
        void __cdecl Commit(_In_ ID3D12CommandQueue* in_command_queue);
        /// This frees up any unused memory.
        /// If you want to make sure all memory is reclaimed, idle the GPU before calling this.
        /// It is not recommended that you call this unless absolutely necessary (e.g. your
        /// Memory budget changes at run-time, or perhaps you're changing levels in your game.)
        void __cdecl GarbageCollect();
        /// Memory statistics
        GraphicsMemoryStatistics __cdecl GetStatistics();
        void __cdecl ResetStatistics();

    private:
        /// Private implementation.
        class Impl;

    private:
        std::unique_ptr < Impl > _impl;
    };
}
